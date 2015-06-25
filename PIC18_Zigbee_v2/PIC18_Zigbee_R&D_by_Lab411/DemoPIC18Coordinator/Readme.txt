//Writen by dat_a3cbq91
//13/8/2012
//instruction for project

Node nay co nhiem vu sau :
+ Khoi tao mang ZigBee

Node co tinh nang sau:
- Tinh nang thu thap nhiet do-do am: can dinh nghia USE_SHT10 trong file zigbee.def
  Mot cach dinh ki, khi timer1 tran 25 lan, MCU se tien hanh doc du lieu tu cam
  bien SHT10 thong qua giao tiep I2C. Neu lan doc du lieu sau ma co su chenh lech
  1 do C so voi lan doc truoc thi se tien hanh gui du lieu ve Router-emboard
- Tinh nang phat hien khoi: can dinh nghia USE_MQ6 trong file zigbee.def
  Mot cach dinh ki, khi timer1 tran 25 lan se kich hoat bo ADC lay mau tin hieu tu
  MQ6. Khi da lay mau xong, se kich hoat cho gia tri dem so lan tran timer1 len 24
  nhu vay, gan nhu ngay sau do, timer1 tiep tuc tran, luc nay, ADC se lay mau dien ap nguon.
- Tinh nang do dien ap nguon: Can dinh nghia ENERGY_TRACKING. Sau khi lay mau tin hieu
  tu cam bien khoi, ADC se lay mau dien ap nguon.
- Tinh nang bat tat bom tuoi: can dinh nghia USE_CONTROL_PUMP. Khi nhan duoc lenh,
  neu duoc dinh nghia USE_CONTROL_PUMP, no se tien hanh gui cac ki tu A,B,C,S xuong
  mach dieu khien dong co thong qua UART. Sau do, no se tien hanh gui ban tin thong
  bao trang thai ve router-emboard. Nhung neu khong dinh nghia USE_CONTROL_PUMP thi
  no se tien hanh gui thong bao tro lai router-emboard la no khong ho tro viec bat/tat
  van tuoi.

//13/10/2012
//instruction for project
After rewrite specification, WSAN group decide modify source code to make operation
of Zigbee nodes are more suitable with all system.