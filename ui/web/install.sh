#!/bin/bash

sudo mount -t vfat /dev/sdc1 /mnt/vfat
sudo rm -f /mnt/vfat/favicon.ico
sudo rm -f /mnt/vfat/index.html
sudo rm -f /mnt/vfat/css/*
sudo rm -f /mnt/vfat/js/*
sudo cp ./dist/favicon.ico /mnt/vfat/
sudo cp ./dist/index.html /mnt/vfat/
sudo cp ./dist/css/* /mnt/vfat/css/
sudo cp ./dist/js/* /mnt/vfat/js/
sudo umount /mnt/vfat
