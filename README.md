# Báo cáo: Gửi dữ liệu MQTT qua Sim 4G 

# Mô tả
Dự án sử dụng module Sim 4G để gửi bản tin lên MQTT Broker. Hiện tại em đã thực hiện thành công phần gửi dữ liệu qua Sim 4G, còn phần LoRa chưa có module để test tiếp.

# Phần cứng sử dụng
- STM32F103C8T6
- Sim A7670C 
- ST-Link V2
- Dây jump
- Nguồn 3.3V / 5V

# Chức năng
- Kết nối Sim 4G và gửi dữ liệu đến MQTT server (Thingsboard)
- Dữ liệu gửi lên gồm: nhiệt độ, độ ẩm,..

# Hướng dẫn sử dụng
1. Kết nối phần cứng :
   - Kết nối chân TX, RX của STM32F103c8t6 tới chân RX, TX của module SIM (giao thức uart).
   - Cấp nguồn 4V-5V, 1-2A cho module Sim.
   
2. Tạo tài khoản trên Thingsboard.cloud:
   - Tạo 1 divice (thiết bị) trên thingsboard.cloud (MQTT broker)  để trao đổi dữ liệu giữa sever và module Sim.
   - Mỗi divice sẽ có các thông tin về Topic, ID , user và pass tương ứng để module Sim gửi data đúng nơi tránh nhầm lẫn dữ liệu.
   - Tạo Dashboards ( giao diện hiện thị). Tạo widget để hiện thị dữ liệu, để hiện thị nhiệt độ thì widget cần ứng với 1 divice  và key:temperature. 
3.  Kết nối MQTT
   3.1. Sử dụng ATcommand:
   - Sử dụng lệnh "AT+CMQTTSTART" để bắt đầu kết nối MQTT. Nếu thành công sẽ trả về
   " OK 
    +CMQTTSTART: 0 "
     nếu thất bại sẽ trả về
   " OK 
    +CMQTTSTART: <errcode>" ( <errcode> Là các list danh sách lỗi ) 
     <errcode> Là các list danh sách lỗi
     Nếu MQTT đã được start.
    " ERROR " 
    - Sử dụng "AT+CMQTTACCQ: <client_index>, <clientID>,<server_type>" . được sử dụng để có được client MQTT. Nó phải được gọi trước tất cả các lệnh về kết nối MQTT và sau AT + CMQTTSTART
     <client_index>: 0-1
     <clientID> : ID của divice trên thingsboard.cloud
     <server_type>: 0 : giao thức TCP-IP, 1: SSL-TLS: Tạo kết nối bảo mật giữa Serve và thiết bị
    - Sử dụng  "AT+CMQTTCONNECT: 0,<server_addr>,<keepalive_time>,<clean_session>,<user_name>,<pass_word>" để kết nối MQTT tới server
     <server_addr>: Địa chỉ sever :" tcp://Thingsboard.cloud"
     <keepalive_time>:Thời gian duy trì kết nối, trong khoảng 1- 64800s không có dữ liệu gửi từ client lên server thì sẽ gửi 1 bản tin duy trì kết nối.
     <clean_session>: 1:Loại bỏ đăng ký client sau khi ngắt kết nối
     <user_name>,<pass_word>: thông tin Tài khoản và mật khẩu của divice trên thingsboard.cloud
  3.2 Pulish dữ liệu lên server
     - Sử dụng lệnh "AT+CMQTTTOPIC=<client_index>,<req_length>". Để nhập topic mà client gửi dữ liệu lên( publish) 
       <client_index>: thông số định nghĩa 1 client.
       <req_length>: Độ dài chuỗi Topic
     - Nếu thành công trả về chuỗi :
    " > 
     <input data here> 
      OK "
    <input data here> :  topic
    - Sử dụng lệnh:"AT+CMQTTPAYLOAD=<client_index>,<req_length>" nhập độ dài chuỗi dữ liệu (định dạng json)
    <client_index>: thông số định nghĩa 1 client.
    <req_length>: độ dài chuỗi dữ liệu (dạng json). giả sử chuỗi định dạng {"temperature":35"} có độ dài là 19
    - Sử dụng lệnh :"AT+CMQTTPUB=<client_index>,<qos>,<pub_timeout> . Để puslish dữ liệu lên server
  4.Kết quả
 4.1 hiện thị nhiệt độ , độ ẩm trên dashboard 

