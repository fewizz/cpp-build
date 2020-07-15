source uninstall.sh $1/
source build.sh $1 && { \
echo "installing..."; \
cp -avr include $1/; \
cp -avr share $1/; }