# HW03 - HEQ & Histogram Matching

- [ ]  과   목 : Computer Vision
- [ ]  작성일 : 2020.09.29
- [ ]  작성자 : 2015741076 김진원

    ![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled.png)

# 1. Histogram Equalization

---

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%201.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%201.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%202.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%202.png)

- 파일을 클릭하여 "단지.bmp" 사진을 불러온다.

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%203.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%203.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%204.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%204.png)

- Histogram Equalization 버튼을 클릭하면 우측 이미지와 같이 밝아지는 것을 볼 수 있다.

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%205.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%205.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%206.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%206.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%207.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%207.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%208.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%208.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%209.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%209.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2010.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2010.png)

- 이를 히스토그램으로 그려보면 위와 같다.

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2011.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2011.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2012.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2012.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2013.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2013.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2014.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2014.png)

- 흑백이미지인 개코원숭이 사진을 가지고 HE를 실행시키면 위와 같이 나온다.

# 2. Histogram Matching

---

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2015.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2015.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2016.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2016.png)

- 파일을 클릭하여 시골풍경 이미지를 불러온다.

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2017.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2017.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2018.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2018.png)

- "시골풍경-s"를 "시골풍경-t"의 histogram 분포로 histogram matching을 했을 경우

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2019.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2019.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2020.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2020.png)

- "시골풍경-t"를 "시골풍경-s"의 histogram 분포로 histogram matching을 했을 경우

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2021.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2021.png)

![HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2022.png](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Untitled%2022.png)

- "악어와 펠리칸"을 "오일"의 histogram 분포로 histogram matching을 했을 경우

### 소스파일

[Homeworks_CV.zip](HW03%20-%20HEQ%20&%20Histogram%20Matching%201577d03373e742d2bcb40b5ef1c57cca/Homeworks_CV.zip)