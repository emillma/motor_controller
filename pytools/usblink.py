import usb.core
import usb.util
dev = usb.core.find(find_all=True)
for d in dev:
    print (usb.util.get_string(d,128,d.iManufacturer))
    print (usb.util.get_string(d,128,d.iProduct))
    print ((d.idProduct,d.idVendor))