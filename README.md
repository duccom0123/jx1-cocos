- 👋 Xin chào, tôi là @duccom0123
- 👀 Đây là bản jx1 cocos2dx226
- ⚡ Sửa đổi:
    * Sửa đổi để build proj.win32, để ae có thể debug trực tiếp trên máy tính để quá trình dev trở lên nhanh hơn
- ⚡ Cách sử dụng:
    * B1: git clone https://github.com/duccom0123/jx1-cocos.git
    * B2: copy jxclient vào trong cocos2dx226\projects (ae nên backup lại file cũ của mình cho chắc nhé)
    * B3: copy toàn bộ các pak vào thư mục jxclient\data\data
    * B4: package_vn.ini tôi đã bỏ mã hóa ip tại file jxclient\Classes\gamescene\KUpdateLayer.cpp dòng 147-152 (bạn có thể sửa lại nếu muốn mã hóa ip)(anh em cần sửa lại ip nhé)
    * B5: sửa đổi szRootPath tại dòng 27 của file jxclient\Classes\engine\KFilePath.cpp thành đường dẫn data của bạn(ở B3)
    * B6: mở jxclient\proj.win32\jxclient.sln và build với vs2010

- Hiện tại chỉ mới load đến phần chọn các nút khởi đầu, anh em cần phải sửa hoặc thêm các spr cần thiết để vào các scene tiếp theo!
