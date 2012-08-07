<?php

/**
 * @file
 * Renders the body portion of a FASTA views data export
 */
$defline_tpl = $variables['options']['display']['defline_fields'];
$num_bases_per_line = $variables['options']['display']['num_bases_per_line'];
$use_residues = $variables['options']['display']['use_residues'];
$residues_colname = $variables['options']['display']['residues_colname'];

if(!$num_bases_per_line){
   $num_bases_per_line = 50;
}

// foreach row in the views table
foreach ($themed_rows as $index => $fields) {
  $defline = array();
  $residues = '';
  
  // if we're using the residues as is then we assume the residues are already
  // formatted in FASTA format.   We just need to print
  if ($use_residues) {
     print "$fields[$residues_colname]\r\n";
  }
  // if we're not using the residues as is then wrap the residues
  // and generate a proper FASTA format
  else {
  
    foreach ($fields as $key => $value) {

      // if the setup indicates, wrap the sequence 
      if (strcmp($key, $residues_colname) == 0) {
        $residues = wordwrap($value, $num_bases_per_line, "\r\n", TRUE);
      }

      // set the FASTA header
      if (strcmp($key, 'defline') == 0) {
        $defline = $value;
      }
    }

    // print the FASTA record
    print ">$defline\r\n";
    print "$residues\r\n";
  }
}


