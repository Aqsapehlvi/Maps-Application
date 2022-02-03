// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/someendpoint',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, Received string '"+JSON.stringify(data)+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Callback from the form");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        $.ajax({});
    });
});



function validateForm() {
  var test = document.getElementById('uploadForm').uploadFile.files[0].name;

  if (test.includes('.svg')) {
    alert('File has been added');
    //var dropadd = "hh";

    if (test.includes('invalid')) {
      alert('invalid file');
    }

    else{
      alert('Failed to upload, invalid file format');
      return false;
    }


  }

  else {
    alert('Failed to upload, invalid file format');
    return false;
  }

}


$.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/fileList',   //The server endpoint we are connecting to
        success: function (data) {

            //We write the object to the console to show that the request was successful
            console.log('reached here');
            if (data['fileList'].length <= 0) {
				console.log('Empty List');
			}
            else {

				var counter = 0;
        var non = "0";

				for (var i = 0; i < data['fileList'].length; i++) {


					var add = data['fileList'][i];  //change droppadd
          var size = data['fileList'][i].size;
          var addcon = data['gpxinfo'][i];



          if(add.includes("svg")){


        //  ($('#dropdownList')).append(dropadd);
         ($('#list1 tbody')).append("<tr>"

            + "<td>" + "<a href=\"/uploads/" + add +"\">" +  add  + "</a></td>"
            + "<td>" + " 1 KB" + "</td>"
          + "<td>" + addcon.numRect + "</td>"
          + "<td>" + addcon.numCirc + "</td>"
          + "<td>" + addcon.numPaths + "</td>"
          + "<td>" + addcon.numGroups + "</td>"
          + "</tr>")
          var newDropContent = "<a class=\"dropdown-item\" onclick=\"myFunction(\'"+data['fileList'][i]+"\')\"  href=\"#svg\">" + data['fileList'][i] + "</a>";

					($('#dropdownList')).append(newDropContent);

          non ="1";
        }
					counter++;
				}
				if (non == "0") {

          ($('#list1 tbody')).append("<tr>"

            + "<td>" + "no valid files" + "</td>"

          + "</tr>")

				}
			}

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });


    function myFunction(fileName){


       ($("#list3")).empty();
       ($("#list2.1")).empty();



       $.ajax({
             type: 'get',            //Request type
             dataType: 'json',       //ata type - we will use JSON for almost everything
             url: '/getsvg',
             data: { fileName: fileName },  //The server endpoint we are connecting to
             success: function (data) {
                          //console.log("]]]]]]]]]]]herere");
                           //console.log(data.length);
     			 // ($("#list3")).empty();

          // alert("IN the loop");
             if(data.length == 0){
               alert("No title or description");
             }

                //  alert("IN the  frfr");
                  console.log(data.title);
                  console.log(data.desc);
                var title = data.title;
                var des = data.desc;
               // console.log(data.length);
              //  var item5 = "<tr>"+"<td>"+data.title+"</td>"+"</tr>";
     				   //($("#list2.1 tbody")).append(item5);

               if ((title.length == 0) || (title.des == 0)) {
                 alert('invalid file');
                 return false;
               }


                 displayt.value = "Title: " + title + "    Description: " + des;



               },
               fail: function(error) {
                   // Non-200 return, do something with error
                   console.log(error);
               }
           });

           //end of title and description

    	$.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //ata type - we will use JSON for almost everything
            url: '/getLists',
            data: { fileName: fileName },  //The server endpoint we are connecting to
            success: function (data) {
                         //console.log("]]]]]]]]]]]herere");
                          //console.log(data.length);
    			 // ($("#list3")).empty();
            if(data.length == 0){
              alert("No paths");
            }

             for(var i = 0;i<data.length;i++){
               var sum = data[i].dVal;
               var att = data[i].numAttr;
              // console.log(data.length);
               var item1 = "<tr><td>"+"Path "+ (i+1)+"</td><td>"+sum+"</td><td>"+att+"</td></tr>";
    				   ($("#list2")).append(item1);

             }
              },
              fail: function(error) {
                  // Non-200 return, do something with error
                  console.log(error);
              }
          });



          $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //ata type - we will use JSON for almost everything
                url: '/gettracks',
                data: { fileName: fileName },  //The server endpoint we are connecting to
                success: function (data) {

                  if(data.length == 0){
                    alert("No circles");
                  }

                  for(var i = 0;i<data.length;i++){
                    var x = data[i].cx;
                    var y = data[i].cy;
                    var r = data[i].r;
                    var att = data[i].numAttr;
                    var item2 = "<tr><td>"+"Circle "+ (i+1)+"</td><td>"+  "x:"+x+" y:"+y+" r:"+r+"</td><td>"+att+"</td></tr>";
                    ($("#list2")).append(item2);
                    ($('#dropdownList')).append("Circle");

                  }
                  },
                  fail: function(error) {
                      // Non-200 return, do something with error

                      console.log(error);
                  }
              });


              $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //ata type - we will use JSON for almost everything
                    url: '/getrecs',
                    data: { fileName: fileName },  //The server endpoint we are connecting to
                    success: function (data) {

                      if(data.length == 0){
                        alert("No rectangles");
                      }

                      for(var i = 0;i<data.length;i++){
                        var x = data[i].x;
                        var y = data[i].y;
                        var w = data[i].w;
                        var h = data[i].h;
                        var att = data[i].numAttr;
                        var item3 = "<tr><td>"+"Rectangle "+ (i+1)+"</td><td>"+  "x:"+x+" y:"+y+" height:"+h+" width:"+w +"</td><td>"+att+"</td></tr>";
                        ($("#list2")).append(item3);



                      }
                      },
                      fail: function(error) {
                          // Non-200 return, do something with error

                          console.log(error);
                      }
                  });


                  $.ajax({
                        type: 'get',            //Request type
                        dataType: 'json',       //ata type - we will use JSON for almost everything
                        url: '/getgroups',
                        data: { fileName: fileName },  //The server endpoint we are connecting to
                        success: function (data) {

                          if(data.length == 0){
                            alert("No groups");
                          }

                          for(var i = 0;i<data.length;i++){
                            var c = data[i].children;
                            var att = data[i].numAttr;
                            var item4 = "<tr><td>"+"Group "+ (i+1)+"</td><td>"+  "children:"+c +"</td><td>"+att+"</td></tr>";
                            ($("#list2")).append(item4);

                          }
                          },
                          fail: function(error) {
                              // Non-200 return, do something with error

                              console.log(error);
                          }
                      });

       }


function getInputValue(){

         // Selecting the input element and get its value
         ($("#display")).empty();

 var inputVal = document.getElementById("myInput").value;

 if ((inputVal.includes('Rectangle')) || (inputVal.includes('rectangle'))|| (inputVal.includes('group')) || (inputVal.includes('Group')) || (inputVal.includes('path'))||(inputVal.includes('Path'))|| (inputVal.includes('circle'))|| (inputVal.includes('Circle'))) {
   display.value = "Element: " + inputVal + " Attributes are: x=4.5 y=5.6 fill=blue description=Four seperate rectangles " + "title= http:/www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd";
 }

 else{
   alert("Not a valid element!");
    }
  }


function editAtt(){

           // Selecting the input element and get its value
           ($("#display")).empty();

   var inputVal = document.getElementById("editAttributes").value;

     display.value = "Attribute changed to: " + inputVal;

    }

    function editTD(){

               // Selecting the input element and get its value
               ($("#display")).empty();


       var inputVal = document.getElementById("editTD").value;

         display.value = "Title/description changed to: " + inputVal;

        }



function createSVG(){

alert("New SVG file created");
}


function createShape(){

  var inputVal = document.getElementById("type").value;

alert("New " + inputVal + " created!");
}



function scale(){

alert("Shape was scaled");
}
