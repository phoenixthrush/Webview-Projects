@echo off
cd src

setlocal

set "filePath=site\index.html"
set "outputPath=html.c"
set "arrayName=site_index_html"

powershell -C ^
  "$bytes = [System.IO.File]::ReadAllBytes('%filePath%');" ^
  "$byteArray = $bytes | ForEach-Object { '0x{0:x2}' -f $_ };" ^
  "$byteArrayString = [string]::Join(', ', $byteArray);" ^
  "$header = 'unsigned char %arrayName%[] = {' + [System.Environment]::NewLine + '    ' + $byteArrayString + [System.Environment]::NewLine + '};' + [System.Environment]::NewLine;" ^
  "$length = 'unsigned int %arrayName%_len = ' + $bytes.Length + ';' + [System.Environment]::NewLine;" ^
  "$header | Out-File '%outputPath%' -Encoding ascii;" ^
  "$length | Out-File '%outputPath%' -Encoding ascii -Append;"

endlocal

cmake -G Ninja -B build -S . -D CMAKE_BUILD_TYPE=Release
cmake --build build

strip .\build\bin\example.exe
powershell -C "Get-ChildItem '%directory%' -Recurse | Measure-Object -Property Length -Sum | ForEach-Object { '{0:N2} MB' -f ($_.Sum / 1MB) }"

.\build\bin\example.exe
cd ..