# 📝 AVR 자율주행차 프로젝트

---

# 📚 목차

- 프로젝트 개요
- 외관 사진
- 회로도
- BOM Table
- FSM
- 세부 기능
- 작동 영상
- 사용한 요소 기술
- 결론

---

# 프로젝트 개요

### 프로젝트 배경 및 목적
- 본 프로젝트는 8비트 AVR 마이크로컨트롤러인 ATmega128을 활용하여 실시간 센서 데이터 처리와 모터 드라이버 제어가 결합된 스마트 자율주행 모형차를 설계하고 구현하는 것을 목적으로 함
- 단순한 모형차 구동을 넘어, 시스템에 적용된 핵심 임베디드 SW 기술들을 하드웨어 레벨에서 명확히 이해하고 제어 능력을 체득하는 데 중점을 둠

---

# 외관 사진

<img width="563" height="202" alt="image" src="https://github.com/user-attachments/assets/2f0a1e3f-be79-4b49-aa9c-17a273b0bbe6" />

---

# 회로도

<img width="2349" height="1620" alt="회로도" src="https://github.com/user-attachments/assets/abed4ed8-7a08-4ad2-b29b-aed40c5fa28b" />

---

# BOM Table

<img width="456" height="536" alt="image" src="https://github.com/user-attachments/assets/497f03c2-c343-434b-8b74-16b19c6dbc01" />

---

# FSM

<img width="2042" height="1126" alt="image" src="https://github.com/user-attachments/assets/59953a56-db65-4196-8df7-a8af0dbd9d2b" />

---

# 세부 기능

### 버튼 조작
- 주행 종료 이후 버튼 조작에 의해 FND에 결과 조회 가능
- FND1: 총 주행 시간
- FND2: 후진 횟수 / 좌회전 횟수 / 우회전 횟수
- FND2:: 정시 상태인 경우 ‘- - - -’

### 자율 주행
- 자율주행 모드 동작 시, FND에 현 상태 확인 가능
- FND1: 현 주행 시간
- FND2: 진행 방향
- 전진: 시계 방향 회전, 후진: 반시계 방향 회전
- 좌회전: ‘[ [ [ [’, 우회전: ‘] ] ] ]’

---

# 작동 영상
|버튼 조작 영상|자율주행 동작 영상|
|:---:|:---:|
| [![버튼 조작 영상](https://img.youtube.com/vi/vm7X0AhSNZI/hqdefault.jpg)](https://youtube.com/shorts/vm7X0AhSNZI) | [![자율주행 동작 영상](https://img.youtube.com/vi/aPnieMM_SbA/hqdefault.jpg)](https://youtube.com/shorts/aPnieMM_SbA) |

---

# 사용한 요소 기술
- UART 통신
- Bluetooth
- PWM
- Polling/Interrupt

---

# 결론

### AVR 이해도 향상
- ATmega128 데이터 시트를 보며 요구사항들을 구현함으로써 AVR이해도가 향상

### 임베디드 SW 기술 이해도 향상
- 하드웨어 레벨에서의 회로 설계부터 제어까지 자율주행차를 구현함으로써 임베디드 SW 기술 이해도가 향상

### 데이터 시트의 중요도 체감
- 초기 회로 설계 과정에서 UART 통신에 사용되는 핀을 FND로 사용했기에 FND가 비정상적으로 동작되는 현상을 체감
<img width="1996" height="88" alt="image" src="https://github.com/user-attachments/assets/663aa85a-9e93-46b0-88c6-484ffaf301fb" />
