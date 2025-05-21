Dự án này là một hệ thống đăng nhập và đăng ký tài khoản và quản lý ví điểm thưởng, được phát triển bằng ngôn ngữ C/C++ kết hợp với cơ sở dữ liệu SQLite.
Mục tiêu của dự án là hỗ trợ sinh viên Học viện Công nghệ Bưu chính Viễn thông (PTIT) trong việc học tập và thực hành môn học "Ngôn ngữ lập trình C++" vào năm 2025.


## 🔐 Các thành viên tham gia dự án
- Nguyễn Huỳnh Đức - K24DTCN181: Xử lý tính năng đăng nhập, view history
- Lê Hồng Sơn - K24DTCN225: Thiết kế database, tính năng đăng ký
- Đỗ Việt Hưng - K24DTCN199 : Xử lý đổi mật khẩu, otp
- Nguyễn Thế Hưng - K24DTCN198: Xử lý tính năng chuyển tiền, cập nhật thông tin


## 🧩 Tính năng chính

Quản lý người dùng và phân quyền (người dùng thường & quản lý)
Cho phép đăng ký, đăng nhập, thay đổi mật khẩu, xác thực OTP
Quản lý ví điểm, thực hiện chuyển điểm giữa các ví
Ghi log lịch sử giao dịch
Sử dụng cơ sở dữ liệu SQLite tích hợp


## 🔐 Tài khoản mặc định

- *Admin*
  - Tên đăng nhập: ptit2025
  - Mật khẩu: ptit2025

- *User*
  - Tên đăng nhập: user2025
  - Mật khẩu: user2025

## 🗂 Cấu trúc thư mục
```
├── controller/ # Xử lý logic nghiệp vụ
├── model/ # Định nghĩa cấu trúc dữ liệu và truy vấn SQLite
├── view/ # Giao diện dòng lệnh
├── utils/ # Các hàm tiện ích
├── crypto/ # Mã hóa mật khẩu
├── sqlite/ # Thư viện SQLite tích hợp
├── main.cpp # Điểm khởi đầu của chương trình
├── CMakeLists.txt # Tập tin cấu hình CMake
└── README.md # Tài liệu mô tả dự án
```

## 🛠 Hướng dẫn cài đặt và chạy

### Yêu cầu hệ thống

Trình biên dịch C/C++ hỗ trợ CMake (GCC, Clang hoặc MSVC)
CMake >= 3.10
C++17 hoặc cao hơn


### Cài đặt

```bash
git clone https://github.com/nguyenduc96/PTIT-CPP-2025.git
cd PTIT-CPP-2025
mkdir build && cd build
cmake ..
make
📌 Ghi chú
Dự án sử dụng SQLite tích hợp sẵn, không cần cài thêm phần mềm cơ sở dữ liệu

Mật khẩu được mã hóa SHA256 để tăng cường bảo mật

Cấu trúc mô-đun rõ ràng, dễ bảo trì và mở rộng