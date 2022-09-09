#!/bin/bash

sudo mount -t vfat /dev/sdc1 /mnt/vfat
sudo rm -f /mnt/vfat/favicon.ico
sudo rm -f /mnt/vfat/index.html
sudo rm -f /mnt/vfat/css/*
sudo rm -f /mnt/vfat/js/*
sudo rm -f /mnt/vfat/fonts/*
sudo rm -f /mnt/vfat/icons/*
sudo cp ./dist/spa/favicon.ico /mnt/vfat/
sudo cp ./dist/spa/index.html /mnt/vfat/
sudo cp ./dist/spa/css/* /mnt/vfat/css/
sudo cp ./dist/spa/js/* /mnt/vfat/js/
sudo cp ./dist/spa/fonts/* /mnt/vfat/fonts/
sudo cp ./dist/spa/icons/* /mnt/vfat/icons/
sudo umount /mnt/vfat
