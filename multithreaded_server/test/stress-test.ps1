param(
    [int]$Waves = 10,
    [int]$RequestsPerWave = 50,
    [string]$Url = "http://localhost:8080/"
)

Write-Host "Stress testing $Url"
Write-Host "$Waves waves x $RequestsPerWave requests = $($Waves * $RequestsPerWave) total requests"
Write-Host ""

for ($wave = 1; $wave -le $Waves; $wave++) {
    Write-Host "Wave $wave"

    1..$RequestsPerWave | ForEach-Object {
        Start-Job -ArgumentList $Url {
            param($Url)

            try {
                $response = Invoke-WebRequest $Url -UseBasicParsing

                if ($response.StatusCode -eq 200) {
                    "SUCCESS"
                } else {
                    "BAD STATUS: $($response.StatusCode)"
                }
            } catch {
                "FAILED: $($_.Exception.Message)"
            }
        }
    }

    $results = Receive-Job * -Wait
    $results | Group-Object

    Remove-Job * -Force

    Write-Host ""
}

Write-Host "Stress test complete."