rm -rf debian
rm -rf obj-x86_64-linux-gnu
rm -rf ../*topsens-explorer*.deb
bloom-generate rosdebian --os-name ubuntu --ros-distro kinetic
fakeroot debian/rules binary
rm -rf debian
rm -rf obj-x86_64-linux-gnu