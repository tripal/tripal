<?php

/**
 *
 * @ingroup views_field_handlers
 * @ingroup tripal_core
 */
class views_handler_field_dbxref_accession_link extends views_handler_field {
 function construct() {
   parent::construct();
   $this->additional_fields['accession'] = 'accession';
   $this->additional_fields['db_id'] = 'db_id';

 }

 function query() { 
   $this->ensure_my_table();
   $this->add_additional_fields(); 
 }

 function render($values) { 

   $accession = $values->{$this->aliases['accession']}; 
   $db_id = $values->{$this->aliases['db_id']};

   if (!empty($db_id) AND !empty($accession)) {
     $previous_db = tripal_db_set_active('chado');
     $result = db_fetch_object(db_query('SELECT urlprefix FROM db WHERE db_id=%d', $db_id));
     tripal_db_set_active($previous_db);   
     $urlprefix = $result->urlprefix;

     if (!empty($urlprefix)) {
       $accession_link = $urlprefix . $accession;
       return($accession_link); 
     } else {
       return $accession;
     }
   } else {
     return $accession;
   }
 } 
} 