---
permalink: /PROJECT_SCREENSHOTS/
---

<body>
  <h1 style="margin-top: 0px;">Project Screenshots</h1>
  <p>Following are some screenshots taken from the project:</p>
    
  <div class="slideshow-container">
  
  <div class="mySlides fade">
    <div class="numbertext">1 / 3</div>
    <img src="https://user-images.githubusercontent.com/66676402/124522619-595adc00-de0d-11eb-87b5-421a0f93410d.png">
    <div class="text">MAIN MENU</div>
  </div>
  
  <div class="mySlides fade">
    <div class="numbertext">2 / 3</div>
    <img src="https://user-images.githubusercontent.com/66676402/124522715-aa6ad000-de0d-11eb-86a2-596f4e3e3245.png">
    <div class="text">DATA MANAGER MENU</div>
  </div>
  
  <div class="mySlides fade">
    <div class="numbertext">3 / 3</div>
    <img src="https://user-images.githubusercontent.com/66676402/124522878-2107cd80-de0e-11eb-8c25-f268305e7ad2.png">
    <div class="text">SORT DATA MENU</div>
  </div>
  
  <a class="prev" onclick="plusSlides(-1)">&#10094;</a>
  <a class="next" onclick="plusSlides(1)">&#10095;</a>
  </div>
  <br>
  
  <div style="text-align:center">
    <span class="dot" onclick="currentSlide(1)"></span> 
    <span class="dot" onclick="currentSlide(2)"></span> 
    <span class="dot" onclick="currentSlide(3)"></span>
  </div>

  <script>
    {% include scripts/slideshow.js %}
  </script>
</body>
