param(
	[string]$CitiesPath = "cities.csv",
	[string]$RoutesPath = "routes.csv",
	[string]$OutputPath = "routes_modern.csv"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$culture = [System.Globalization.CultureInfo]::InvariantCulture
$today = Get-Date -Format "yyyy-MM-dd"

function Get-DistanceKm {
	param(
		[double]$Lat1,
		[double]$Lon1,
		[double]$Lat2,
		[double]$Lon2
	)

	$radius = 6371.0
	$toRadians = [Math]::PI / 180.0
	$dLat = ($Lat2 - $Lat1) * $toRadians
	$dLon = ($Lon2 - $Lon1) * $toRadians
	$a = [Math]::Sin($dLat / 2) * [Math]::Sin($dLat / 2) + [Math]::Cos($Lat1 * $toRadians) * [Math]::Cos($Lat2 * $toRadians) * [Math]::Sin($dLon / 2) * [Math]::Sin($dLon / 2)
	$c = 2 * [Math]::Atan2([Math]::Sqrt($a), [Math]::Sqrt(1 - $a))
	return $radius * $c
}

function Get-Jitter {
	param(
		[string]$Text
	)

	$hash = 0
	foreach ($char in $Text.ToCharArray()) {
		$hash = (($hash * 31) + [int][char]$char) % 1000
	}

	return 0.95 + (($hash / 1000.0) * 0.1)
}

function Get-Estimate {
	param(
		[string]$Transport,
		[double]$DistanceKm,
		[string]$Key
	)

	$transportLower = $Transport.ToLowerInvariant()
	$time = 0.0
	$cost = 0.0

	switch ($transportLower) {
		"plane" {
			$time = 0.9 + ($DistanceKm / 850.0)
			if ($DistanceKm -gt 5000) { $time += 1.5 }
			if ($DistanceKm -gt 9000) { $time += 2.5 }
			if ($DistanceKm -gt 13000) { $time += 3.5 }
			$cost = 60.0 + ($DistanceKm * 0.06)
		}
		"train" {
			$time = 0.5 + ($DistanceKm / 105.0)
			$cost = 12.0 + ($DistanceKm * 0.045)
		}
		"bus" {
			$time = 0.75 + ($DistanceKm / 70.0)
			$cost = 5.0 + ($DistanceKm * 0.035)
		}
		"boat" {
			$time = 1.0 + ($DistanceKm / 45.0)
			$cost = 25.0 + ($DistanceKm * 0.04)
		}
		"truck" {
			$time = 0.75 + ($DistanceKm / 60.0)
			$cost = 8.0 + ($DistanceKm * 0.03)
		}
		default {
			$time = 0.75 + ($DistanceKm / 90.0)
			$cost = 15.0 + ($DistanceKm * 0.05)
		}
	}

	$time *= Get-Jitter -Text ($Key + ":time")
	$cost *= Get-Jitter -Text ($Key + ":cost")

	return @{
		TimeHours = [Math]::Round($time, 2)
		CostUSD = [Math]::Round($cost, 2)
	}
}

$cityLookup = @{}
Import-Csv -Path $CitiesPath -Header Country,Capital,Latitude,Longitude | Where-Object { [double]::TryParse($_.Latitude) -and [double]::TryParse($_.Longitude) && $_.Latitude -ne '' -and $_.Longitude -ne '' }} | ForEach-Object {
	$capital = $_.Capital.Trim()
	if ($capital -ne "") {
		$cityLookup[$capital] = [pscustomobject]@{
			Country = $_.Country.Trim()
			Capital = $capital
			Latitude = [double]::Parse($_.Latitude, $culture)
			Longitude = [double]::Parse($_.Longitude, $culture)
		}
	}
}

$modernRoutes = New-Object System.Collections.Generic.List[object]

$rawRoutes = Import-Csv -Path $RoutesPath -Header Origin,Destination,Transport,Time,Cost,Note
foreach ($row in $rawRoutes) {
	$origin = $row.Origin.Trim()
	$destination = $row.Destination.Trim()
	$transport = $row.Transport.Trim()

	if (-not $cityLookup.ContainsKey($origin) -or -not $cityLookup.ContainsKey($destination)) {
		continue
	}

	$originCity = $cityLookup[$origin]
	$destinationCity = $cityLookup[$destination]
	$distanceKm = Get-DistanceKm -Lat1 $originCity.Latitude -Lon1 $originCity.Longitude -Lat2 $destinationCity.Latitude -Lon2 $destinationCity.Longitude
	$estimate = Get-Estimate -Transport $transport -DistanceKm $distanceKm -Key ($origin + "->" + $destination + ":" + $transport)

	$modernRoutes.Add([pscustomobject]@{
		Origin = $origin
		Destination = $destination
		Transport = $transport
		TimeHours = $estimate.TimeHours.ToString("0.##", $culture)
		CostUSD = $estimate.CostUSD.ToString("0.##", $culture)
		LastUpdated = $today
		Note = "Estimated from geodesic distance model"
	})
}

$modernRoutes | Export-Csv -Path $OutputPath -NoTypeInformation -Encoding UTF8

Write-Host ("Wrote " + $OutputPath + " with " + $modernRoutes.Count + " routes.")
