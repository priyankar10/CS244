<?php
$text = file_get_contents("php://input");
$textarray = explode("\n",$text); 
$file = fopen("assignment2.txt","a+");
foreach($textarray as $line){
      fputcsv($file,explode(",",$line));
 }
fclose($file);
?>