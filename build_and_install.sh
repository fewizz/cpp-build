bash uninstall.sh $1/
bash build.sh $1/bin/cxx-exec.exe
echo "installing..."
cp -avr include $1/
cp -avr share $1/