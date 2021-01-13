# HW02 - Otsu’s thresholding algorithm & Labeling

- [ ]  과   목 : Computer Vision
- [ ]  작성일 : 2020.09.28
- [ ]  작성자 : 2015741076 김진원

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled.png)

# 1. Otsu Thersold

---

### 라벨링을 하기 위해선 이진화 과정이 필수이다. 이를 위해 Otsu Thersold 방식을 사용한다.

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%201.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%201.png)

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%202.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%202.png)

- 파일을 클릭하여 rice.pgm 사진을 불러온다.

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%203.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%203.png)

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%204.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%204.png)

- Otsu Threshold 버튼을 클릭하면 우측 이미지와 같이 이진화된 것을 볼 수 있다.

# 2. Morphology(Opening)

---

### Labeling을 하기 전 먼저 노이즈를 제거해주면 좋기에, 이미지의 맞는 Morphology인 Opening을 사용했다.

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%205.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%205.png)

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%206.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%206.png)

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%207.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%207.png)

- Opening을 선택한 후, Morphology를 클릭하면, 다음과 같이 좁쌀만한 노이즈들이 제거되는 것을 볼 수 있다.

# 3. Labeling

---

### 이제 노이즈를 제거한 이미지를 8-Neighbor로 구현한 Labeling 함수를 적용한다.

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%207.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%207.png)

![HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%208.png](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Untitled%208.png)

- Labeling 버튼을 누르면 우측과 같이 라벨링이 된 이미지를 확인할 수 있다.

### 소스파일

[Homeworks_CV.zip](HW02%20-%20Otsu%E2%80%99s%20thresholding%20algorithm%20&%20Labeling%209ec50646cf304f3d851bde57c06bfa23/Homeworks_CV.zip)