@echo off
cscript /nologo /e:jscript configure.js  "--with-win32ps=shared" "--enable-debug" %*
