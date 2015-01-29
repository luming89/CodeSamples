<?php
include 'PHPExcel/IOFactory.php';
include 'PHPExcel.php';
$DEBUG = false;

$chart_ready = false;
$feedback = "";
$target_dir = "uploads/";
$rows;
$cols;
session_start();
if(isset($_POST["submit"]))
{
  $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
  $_SESSION['filename'] = basename($_FILES["fileToUpload"]["name"]);
  $_SESSION['dir'] = $target_dir;
  $uploadOk = 1;
  $imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);

  // Check file format
  if($imageFileType != "xlsx" && $imageFileType != "xls")
  {
      $feedback = "Sorry, only Excel files are allowed.";
      $uploadOk = 0;
  }
  // Check if $uploadOk is set to 0 by an error
  if ($uploadOk != 0)
  {
      if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
      {
          $feedback = "File uploaded successfully!";


          /** PHPExcel_IOFactory */
          $inputFileName = $target_file;
          $inputFileType = PHPExcel_IOFactory::identify($inputFileName);
          if($DEBUG) echo 'File ',pathinfo($inputFileName,PATHINFO_BASENAME),' has been identified as an ',$inputFileType,' file<br />';
          if($DEBUG) echo 'Loading file ',pathinfo($inputFileName,PATHINFO_BASENAME),' using IOFactory with the identified reader type<br />';
          $objReader = PHPExcel_IOFactory::createReader($inputFileType);
          $objPHPExcel = $objReader->load($inputFileName);

          if($DEBUG) echo '<hr />';

          $sheetData = $objPHPExcel->getActiveSheet()->toArray(null,true,true,true);
          if($DEBUG) var_dump($sheetData);
          $rows = count($sheetData);
          $cols = 0;
          if($rows)
                   $cols = count($sheetData[1]);
          if($cols==2)
          {
            $chart_ready = true;
            $feedback = "File uploaded successfully!";
          }
          else
          {
            $feedback = "Failed to upload: number of columns mismatch";
          }
      }
      else
      {
          $feedback = "Sorry, there was an error uploading your file.";
      }
  }
}

if(isset($_GET['send']))
{
  $inputFileType = PHPExcel_IOFactory::identify($_SESSION['dir'].$_SESSION['filename']);
  $objReader = PHPExcel_IOFactory::createReader($inputFileType);
  $objPHPExcel = $objReader->load($_SESSION['dir'].$_SESSION['filename']);
  $i = 2;
  while($i<=$_GET['rows'])
  {
     $objPHPExcel->getActiveSheet()->setCellValue('A'.$i,$_GET['A'.$i]);
     $objPHPExcel->getActiveSheet()->setCellValue('B'.$i,$_GET['B'.$i]);
     $i++;
  }
  $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, "Excel2007");
  $objWriter->save($_SESSION['dir']."new_".$_SESSION['filename']);
}
?>


<!DOCTYPE HTML>
<!--
	Twenty by HTML5 UP
	html5up.net | @n33co
	Free for personal and commercial use under the CCA 3.0 license (html5up.net/license)
-->
<html>
	<head>
		<title>PA ASSIGNMENT</title>
		<meta http-equiv="content-type" content="text/html; charset=utf-8" />
		<meta name="description" content="" />
		<meta name="keywords" content="" />
		<!--[if lte IE 8]><script src="css/ie/html5shiv.js"></script><![endif]-->
		<script src="js/jquery.min.js"></script>
		<script src="js/jquery.dropotron.min.js"></script>
		<script src="js/jquery.scrolly.min.js"></script>
		<script src="js/jquery.scrollgress.min.js"></script>
		<link rel="stylesheet" href="//code.jquery.com/ui/1.11.2/themes/smoothness/jquery-ui.css">
    <script src="//code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
    <script type="text/javascript" src="js/slider.js"></script>
		<script src="js/skel.min.js"></script>
		<script src="js/skel-layers.min.js"></script>
		<script src="js/init.js"></script>
		<script type="text/javascript" src="js/edit.js"></script>
    <script type="text/javascript">
      var tableData = <?php if($chart_ready) echo json_encode($sheetData);  else echo 0 ?>;
      var rows = <?php if($chart_ready) echo $rows;  else echo 0 ?>;
      var cols = <?php if($chart_ready) echo $cols;  else echo 0 ?>;
      if(cols!=2)
          <?php if($chart_ready) echo 'alert("Number of columns does not equal to 2");'; ?>;
    </script>
    <script type="text/javascript">
      var idown;  // Keep it outside of the function, so it's initialized once.
      function downloadURL(url)
      {
        if (idown)
        {
          idown.attr('src',url);
        } 
        else
        {
          idown = $('<iframe>', { id:'idown', src:url }).hide().appendTo('body');
        }
      }
    </script>
    <script src="js/utilities.js"></script>
    <script type="text/javascript">
      function update()
      {
        var i=2;
        while(i<=rows)
        {
          tableData[i]["B"] = document.getElementById(i+"2").innerText;
          i=i+1;
        }
        sendData();
        if(document.getElementById("download").innerText !="Generate Spreadsheet")
        {
          downloadURL(<?php echo '"'.$target_dir.'new_'.$_SESSION['filename'].'"';  ?>);
          document.getElementById("download").innerText = "Generate Spreadsheet";
        }
        else
        {
          document.getElementById("download").innerText = "Download Spreadsheet";
        }
      }
    </script>
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      function drawChart()
      {
        <?php
        if($chart_ready)
        {
           if($DEBUG) echo 'alert("Drawing Chart!");';
        }
        else
        {
           echo 'alert("No Excel file uploaded");';
           echo 'return;';
        }

        ?>
        var data = google.visualization.arrayToDataTable([
          [document.getElementById("11").innerText, document.getElementById("12").innerText],
          <?php
          if($chart_ready)
          {
              $rows = count($sheetData);
              $i=2;
              while($i<=$rows)
              {
                 $cols = count($sheetData[$i]);
                 $j = 1;
                 echo '[';
                 while($j<=$cols)
                 {
                   echo 'parseFloat(document.getElementById("'.$i.$j.'").innerText),';
                   $j++;
                 }
                 echo '],';
                 $i++;
              }
          }
          ?>
        ]);

        var options =
        {
          title: tableData["1"]["A"]+' vs. '+tableData["1"]["B"]+' comparison',
          hAxis: {title: tableData["1"]["A"], minValue: 0, maxValue: 200},
          vAxis: {title: tableData["1"]["B"], minValue: 0, maxValue: 50000},
          legend: 'none'
        };
        var chart = new google.visualization.ScatterChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>
		<noscript>
			<link rel="stylesheet" href="css/skel.css" />
			<link rel="stylesheet" href="css/style.css" />
			<link rel="stylesheet" href="css/style-wide.css" />
			<link rel="stylesheet" href="css/style-noscript.css" />
		</noscript>
		<!--[if lte IE 8]><link rel="stylesheet" href="css/ie/v8.css" /><![endif]-->
		<!--[if lte IE 9]><link rel="stylesheet" href="css/ie/v9.css" /><![endif]-->
	</head>
	<body class="index">

		<!-- Header -->
			<header id="header" class="alt">
				<h1 id="logo"><a href="index.html">Twenty <span>by HTML5 UP</span></a></h1>
				<nav id="nav">
					<ul>
						<li class="current"><a href="index.php">Welcome</a></li>
						<li class="submenu">
							<a href="">Layouts</a>
							<ul>
								<li><a href="left-sidebar.html">Left Sidebar</a></li>
								<li><a href="right-sidebar.html">Right Sidebar</a></li>
								<li><a href="no-sidebar.html">No Sidebar</a></li>
								<li><a href="contact.html">Contact</a></li>
								<li class="submenu">
									<a href="">Submenu</a>
									<ul>
										<li><a href="#">Dolore Sed</a></li>
										<li><a href="#">Consequat</a></li>
										<li><a href="#">Lorem Magna</a></li>
										<li><a href="#">Sed Magna</a></li>
										<li><a href="#">Ipsum Nisl</a></li>
									</ul>
								</li>
							</ul>
						</li>
						<li><a href="#" class="button special">Sign Up</a></li>
					</ul>
				</nav>
			</header>


		<!-- Banner -->
			<section id="banner">

				<!--
					".inner" is set up as an inline-block so it automatically expands
					in both directions to fit whatever's inside it. This means it won't
					automatically wrap lines, so be sure to use line breaks where
					appropriate (<br />).
				-->
				<div class="inner">

                                        Please upload the Excel file:
                                        <form action="index.php" method="POST" enctype="multipart/form-data">
                                        Select Execl to upload:
                                        <input type="file" name="fileToUpload" id="fileToUpload">
                                        <input type="submit" value="Upload" name="submit">
                                        </form>
                                        <?php
                                              echo $feedback;
                                              echo '<br>';
                                        ?>
                                        <button id = "download" onclick="update()">Generate Spreadsheet</button>


				</div>


			</section>
                        <body>
                              <div class="inner" id="results"> </div>
                              <div class="inner" id="chart_div" style="width: 900px; height: 500px;"></div>
                              <div class="inner" id="slider"> </div>
                              <?php
                                if($chart_ready)
                                {
                                  $rows = count($sheetData);
                                  $i=1;
                                  echo '<table>';
                                  while($i<=$rows)
                                  {
                                      echo '<tr>';
                                      $cols = count($sheetData[$i]);
                                      $j = 1;
                                      while($j<=$cols)
                                      {
                                         echo '<td id='.$i.$j.'>';
                                         if($j==1)
                                             echo $sheetData[$i]["A"];
                                         else
                                             echo $sheetData[$i]["B"];
                                         echo '</td>';
                                         $j++;
                                      }
                                      echo '</tr>';
                                      $i++;
                                  }
                                  echo '</table>';
                                  echo '<script type="text/javascript"> drawChart(); </script>';
                                }
                              ?>

                        </body>
		<!-- Footer -->
			<footer id="footer">

				<ul class="icons">
					<li><a href="#" class="icon circle fa-twitter"><span class="label">Twitter</span></a></li>
					<li><a href="#" class="icon circle fa-facebook"><span class="label">Facebook</span></a></li>
					<li><a href="#" class="icon circle fa-google-plus"><span class="label">Google+</span></a></li>
					<li><a href="#" class="icon circle fa-github"><span class="label">Github</span></a></li>
					<li><a href="#" class="icon circle fa-dribbble"><span class="label">Dribbble</span></a></li>
				</ul>
				<ul class="copyright">
					<li>&copy; Untitled</li><li>Design: <a href="http://html5up.net">HTML5 UP</a></li>
				</ul>
				<ul class="copyright">
          <li>Luming Zhang, luming@cs.wisc.edu</li>
        </ul>
			</footer>
    <!-- My Game Demo 
			<iframe width="640" height="360" src="//www.youtube.com/embed/PuZf0JyCd-Y?feature=player_detailpage" frameborder="0" allowfullscreen></iframe>
    -->  
	</body>
</html>