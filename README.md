<h3> Built for php 8.2.26</h3>

Compile and Build win32ps
=========================
Mark Fleeson (mfleeson@gmail.com)

You will need windows Powershell and Visual Studio 2019 and Git CLI

in a command window type:

`winget search Microsoft.PowerShell`

Run PowerShell.
<pre>
copy/paste 
;
if (-not (Test-Path c:\build-cache)) {
        mkdir c:\build-cache
	cd c:\build-cache
}
;
copy/paste
;
cd c:\build-cache
;
invoke-webrequest https://github.com/php/php-sdk-binary-tools/releases/download/php-sdk-2.3.0/php-sdk-binary-tools-php-sdk-2.3.0.zip -outFile "c:\build-cache\php-sdk-binary-tools-php-sdk-2.3.0.zip"
;
expand-archive php-sdk-binary-tools-php-sdk-2.3.0.zip php-sdk-2.3.0
;
invoke-webrequest "http://windows.php.net/downloads/releases/php-devel-pack-8.2.26-Win32-vs16-x64.zip" -OutFile "c:\build-cache\php-devel-pack-8.2.26-Win32-vs16-x64.zip" -UserAgent 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:125.0) Gecko/20100101 Firefox/125.0'
;
expand-archive php-devel-pack-8.2.26-Win32-vs16-x64.zip php-8.2.26-devel-vs16-x64

Start Visual Studio 2019 Developer Command Prompt
cd back to your github installation containing this README.txt
copy/paste 
;
cd win32ps
;
c:\build-cache\php-8.2.26-devel-vs16-x64\php-8.2.26-devel-vs16-x64\phpize.bat
;
c:\build-cache\php-sdk-2.3.0\php-sdk-binary-tools-php-sdk-2.3.0\phpsdk-vs16-x64.bat
;
.\configure.bat --with-win32ps=shared --enable-debug
;
nmake
;
dir x64\Release_TS
</pre>
