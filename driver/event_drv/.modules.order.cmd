cmd_/mnt/hgfs/VMware_Share/my_project/virtual_lcd/event_drv/modules.order := {   echo /mnt/hgfs/VMware_Share/my_project/virtual_lcd/event_drv/event_drv.ko; :; } | awk '!x[$$0]++' - > /mnt/hgfs/VMware_Share/my_project/virtual_lcd/event_drv/modules.order