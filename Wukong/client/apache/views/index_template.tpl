<html>
<head>
  <style>
    .close {
      position:absolute;
      top:470px;
      left:815px;
      width:0px; 
      height:90px; 
      border:5px solid red;
    }

    .open {
      position:absolute;
      top:560px;
      left:725px;
      width:90px; 
      height:0px; 
      border: 5px solid red;
    }
  </style>
  <script type='text/javascript' src='https://ajax.googleapis.com/ajax/libs/jquery/1.6.4/jquery.min.js'></script>
  <script>
    $(function() {
      function longpoll() {
        $.ajax({
          type: "POST",
          cache: false,
          url: "/people-counter",
          dataType: "json",
          data: {id: 42},
          error: function() {
            console.log('error');

            setTimeout(longpoll, 5*1000);
          },
          success: function(data) {
            console.log(data);

            if (data) {
              if (data.counter) {
                // update counter
                $('#counter').text(data.counter + " people");
              }

              if (data.door) {
                // update door state
                if (data.door == "OPEN") {
                  $('#door').removeClass('close');
                  $('#door').addClass('open');

                  $('#doorText').text('opened');
                }
                else {
                  $('#door').removeClass('open');
                  $('#door').addClass('close');

                  $('#doorText').text('closed');
                }
              }
            }

            setTimeout(longpoll, 1*1000);
            //longpoll();
          },
        });
      }

      longpoll();
    });
  </script>
</head>
<body>
  <h1>People counter</h1>
  <img src="/people-counter/static/img/FloorPlanR441.png" alt="" title="" style="position:absolute; TOP:75px; LEFT:15px; WIDTH:800px; HEIGHT:500px;" />
  <h1 id='counter' style='position:absolute; top:250px; left:400px'>0</h1>
  <div id='door' class='open'></div>
  <h2 id='doorText' style='position:absolute; top:480px; left:840px;'>opened</div>
</body>
</html>
