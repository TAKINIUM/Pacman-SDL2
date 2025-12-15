$ReleaseDir = "PacmanSDL_Release"
$ZipName = "PacmanSDL_Release.zip"

if (Test-Path $ReleaseDir) { Remove-Item -Recurse -Force $ReleaseDir }
if (Test-Path $ZipName) { Remove-Item -Force $ZipName }

New-Item -ItemType Directory -Force -Path $ReleaseDir | Out-Null
Write-Host "Dossier '$ReleaseDir' créé." -ForegroundColor Green

if (-not (Test-Path "bin/PacmanSDL.exe")) {
    Write-Error "Erreur: 'bin/mon_jeu.exe' introuvable. Compile le projet d'abord (make)."
    exit 1
}

Copy-Item "bin/PacmanSDL.exe" -Destination $ReleaseDir
Copy-Item "bin/SDL2.dll" -Destination $ReleaseDir
Write-Host "Exécutable et DLL copiés." -ForegroundColor Green

Copy-Item -Recurse "assets" -Destination $ReleaseDir
Write-Host "Assets copiés." -ForegroundColor Green

Compress-Archive -Path "$ReleaseDir/*" -DestinationPath $ZipName
Write-Host "Compression terminée : $ZipName" -ForegroundColor Cyan

Remove-Item -Recurse -Force $ReleaseDir
Write-Host "Nettoyage terminé. Ton fichier '$ZipName' est prêt !" -ForegroundColor Green
