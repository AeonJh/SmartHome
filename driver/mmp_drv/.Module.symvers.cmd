cmd_/mnt/hgfs/VMware_Share/my_project/virtual_lcd/mmp_drv/Module.symvers := sed 's/\.ko$$/\.o/' /mnt/hgfs/VMware_Share/my_project/virtual_lcd/mmp_drv/modules.order | scripts/mod/modpost -m -a  -o /mnt/hgfs/VMware_Share/my_project/virtual_lcd/mmp_drv/Module.symvers -e -i Module.symvers   -T -
