<?php
$text = file_get_contents("php://input");
$textarray = explode("\n",$text); 
$file = fopen("assignment4_c.txt","a+");
foreach($textarray as $line){
      fputcsv($file,explode(",",$line));
 }
fclose($file);
?>