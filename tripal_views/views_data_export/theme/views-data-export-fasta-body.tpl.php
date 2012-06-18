<?php

/**
 * @file
 * Renders the body portion of a FASTA views data export
 */

//print_r($themed_rows);

// print the first FASTA record header
// this is needed due to the order of the fields
print $defline;

// foreach row in the views table
foreach ($themed_rows as $index => $fields) {
  $defline = array();
  $residues = '';
  foreach ($fields as $key => $value) {

    // wrap the sequence
    if (strcmp($key, 'residues') == 0) {
      $residues = wordwrap($value, 60, "\r\n", TRUE);
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


