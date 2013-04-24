<?php 

/**
 * This is the chado_pub node form callback. The arguments
 * are out of order from a typical form because it's a defined callback 
 */
function chado_pub_node_form($form_state, $node) {   
  tripal_core_ahah_init_form();
  $form = array();
  dpm($form_state);

  $type = node_get_types('type', $node);
  $pub = $node->pub;
  
  $pub_id = $pub->pub_id;
  
  $d_title        = $form_state['values']['title']        ? $form_state['values']['title']       : $pub->title;
  $d_uniquename   = $form_state['values']['uniquename']   ? $form_state['values']['uniquename']  : $pub->uniquename;
  $d_type_id      = $form_state['values']['type_id']      ? $form_state['values']['type_id']     : $pub->type_id->cvterm_id;
  $d_volume       = $form_state['values']['volume']       ? $form_state['values']['volume']      : $pub->volume;
  $d_volumetitle  = $form_state['values']['volumetitle']  ? $form_state['values']['volumetitle'] : $pub->volumetitle;
  $d_series_name  = $form_state['values']['series_name']  ? $form_state['values']['series_name'] : $pub->series_name;
  $d_issue        = $form_state['values']['issue']        ? $form_state['values']['issue']       : $pub->issue;
  $d_pyear        = $form_state['values']['pyear']        ? $form_state['values']['pyear']       : $pub->pyear;
  $d_pages        = $form_state['values']['pages']        ? $form_state['values']['pages']       : $pub->pages;
  $d_miniref      = $form_state['values']['miniref']      ? $form_state['values']['miniref']     : $pub->miniref;
  $d_publisher    = $form_state['values']['publisher']    ? $form_state['values']['publisher']   : $pub->publisher;
  $d_pubplace     = $form_state['values']['pubplace']     ? $form_state['values']['pubplace']    : $pub->pubplace;
  $d_is_obsolete  = $form_state['values']['is_obsolete']  ? $form_state['values']['is_obsolete'] : $pub->is_obsolete;

  // on AHAH callbacks we want to keep a list of all the properties that have been removed 
  // we'll store this info in a hidden field and retrieve it here
  $d_removed = $form_state['values']['removed']; 
  
  // get the defaults first from the database and then from the form_state
  $default_type = $pub->type_id->cvterm_id;

  $form['pub_id'] = array(
    '#type' => 'hidden',
    '#value' => $pub_id,
  );
  
  // get the list of publication types.  In the Tripal publication
  // ontologies these are all grouped under the term 'Publication Type'
  // we want the default to be 'Journal Article'
  $sql = "
    SELECT CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
    WHERE CV.name = 'tripal_pub' and CVTO.name = 'Publication Type'
    ORDER BY CVTS.name ASC 
  ";
  $results = chado_query($sql);
  $pub_types = array();
  while ($pub_type = db_fetch_object($results)) {
    $pub_types[$pub_type->cvterm_id] = $pub_type->name;
    // if we don't have a default type then set the default to be 'Journal Article'
    if (strcmp($pub_type->name,"Journal Article") == 0 and !$d_type_id) {
      $d_type_id = $pub_type->cvterm_id;
    }
  }
  $form['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Publication Type'),
    '#options' => $pub_types,
    '#required' => TRUE,
    '#default_value' => $d_type_id,
  );
  
  // Article Title.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => check_plain($type->title_label),
    '#default_value' => $d_title,
    '#required' => TRUE,
  );
  
  $form['series_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Series Name (e.g. Journal Name)'),
    '#description' => t('Full name of (journal) series.'),
    '#default_value' => $d_series_name,
    '#required' => TRUE,
  );
  $form['pyear'] = array(
    '#type' => 'textfield',
    '#title' => t('Publication Year'),
    '#default_value' => $d_pyear,
    '#required' => TRUE,
  );
  
  $form['uniquename'] = array(
    '#type' => 'textarea',
    '#title' => t('Citation'),
    '#default_value' => $d_uniquename,
    '#description' => t('All publications must have a unique citation. Please enter the full citation for this publication. 
      For PubMed style citations list 
      the last name of the author followed by initials. Each author should be separated by a comma. Next comes 
      the title, followed by the series title (e.g. journal name), publication date (3 character Month, day, 4 
      digit year), volume, issue and page numbers. You may also use HTML to provide a link in the citation.  
      Below is an example: <pre>Medeiros PM, Ladio AH, Santos AM, Albuquerque UP. <a href="http://www.ncbi.nlm.nih.gov/pubmed/23462414" target="_blank">Does the selection of medicinal plants by Brazilian local populations 
        suffer taxonomic influence?</a> J Ethnopharmacol. 2013 Apr 19; 146(3):842-52. PubMed PMID: 23462414</pre>'),
    '#required' => TRUE,
  );
  
  // get publication properties list and create the array that will be used for selecting a property type
  $sql = "
    SELECT CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
    WHERE CV.name = 'tripal_pub' and CVTO.name = 'Publication Details'
    ORDER BY CVTS.name ASC 
  ";
  $prop_types = chado_query($sql);  
  $num_properties = 0;
  $d_properties = array();  
  $properties = array();  
  $removed = array();
  $properties[] = 'Select a Property';
  while ($prop = db_fetch_object($prop_types)) {
    $properties[$prop->cvterm_id] = $prop->name;

    // skip any properties that the user requested to delete through a previous
    // AHAH callback or through the current AHAH callback
    if($d_removed[$prop->cvterm_id . '-0']) {
      continue;
    }
    if($form_state['post']['remove-' . $prop->cvterm_id . '-0']) {
      $d_removed[$prop->cvterm_id . '-0'] = 1;
      continue;
    }
    // if any of the properties match the fields in the pub table then we want to include those 
    // automatically
    if($prop->name == 'Volume' and $d_volume) {
      $d_properties[$prop->cvterm_id][0]['name']  =  $prop->name;     
      $d_properties[$prop->cvterm_id][0]['id']    = $prop->cvterm_id;
      $d_properties[$prop->cvterm_id][0]['value'] = $d_volume;
      $num_properties++;
    }
    if($prop->name == 'Volume Title' and $d_volumetitle) {
      $d_properties[$prop->cvterm_id][0]['name']  =  $prop->name;
      $d_properties[$prop->cvterm_id][0]['id']    = $prop->cvterm_id;
      $d_properties[$prop->cvterm_id][0]['value'] = $d_volumetitle;
      $num_properties++;
    }
    if($prop->name == 'Issue' and $d_issue) {
      $d_properties[$prop->cvterm_id][0]['name']  =  $prop->name;
      $d_properties[$prop->cvterm_id][0]['id']    = $prop->cvterm_id;
      $d_properties[$prop->cvterm_id][0]['value'] = $d_issue;
      $num_properties++;
    }
    if($prop->name == 'Pages' and $d_pages) {
      $d_properties[$prop->cvterm_id][0]['name']  =  $prop->name;
      $d_properties[$prop->cvterm_id][0]['id']    = $prop->cvterm_id;
      $d_properties[$prop->cvterm_id][0]['value'] = $d_pages;
      $num_properties++;
    }
  }
  
  // get the properties for this publication
  if($pub_id) {
    $sql = "
      SELECT CVT.cvterm_id, CVT.name, PP.value, PP.rank
      FROM {pubprop} PP
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = PP.type_id
      WHERE PP.pub_id = %d
      ORDER BY CVT.name, PP.rank
    ";
    $pub_props = chado_query($sql, $pub_id);
    while ($prop = db_fetch_object($pub_props)) {
      // skip properties that were handled above
      if($prop->name == "Volume" or $prop->name == "Volume Title" or 
         $prop->name == "Issue"  or $prop->name == "Pages" or
         $prop->name == "Citation") {
           continue;
      }
      // skip any properties that the user requested to delete through a previous
      // AHAH callback or through the current AHAH callback
      if($d_removed[$prop->cvterm_id . '-' . $prop->rank]) {
        continue;
      }
      if($form_state['post']['remove-' . $prop->cvterm_id . '-' . $prop->rank]) {
        $d_removed[$prop->cvterm_id . '-0'] = 1;
        continue;
      }
      // add new properties that weren't handled yet
      if(array_key_exists($prop->cvterm_id, $properties)) {
        $d_properties[$prop->cvterm_id][$prop->rank]['name']  = $prop->name;
        $d_properties[$prop->cvterm_id][$prop->rank]['id']    = $prop->cvterm_id;
        $d_properties[$prop->cvterm_id][$prop->rank]['value'] = $prop->value;
        $num_properties++; 
      }
    }
  }
  
  $form['removed'] = array(
    '#type' => 'hidden',
    '#value' => $d_removed,
  );
  
  // build the fields for the properties  
  $i = 0;
  foreach ($d_properties as $type_id => $ranks) {
    foreach ($ranks as $rank => $d_property) {
      $form['properties'][$type_id][$rank]["prop_id-$type_id-$rank"] = array(
        '#type'          => 'select',
        '#options'       => $properties,
        '#default_value' => $d_property['id']
      );
      $rows = 2;
      if  (preg_match('/Abstract/', $d_property['name'])) {
        $rows = 10;
      }
      $form['properties'][$type_id][$rank]["prop_value-$type_id-$rank"] = array(
        '#type'          => 'textarea',
        '#default_value' => $d_property['value'],
        '#cols'          => 20,
        '#rows'          => $rows
      );
      
      $form['properties'][$type_id][$rank]["remove-$type_id-$rank"] = array(
        '#type'         => 'image_button',
        '#value'        => t('Remove'),
        '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
        '#ahah' => array(
          'path'    => "tripal_pub/properties/minus/$type_id/$rank",
          'wrapper' => 'chado-pub-details',
          'event'   => 'click',
          'method'  => 'replace',
        ), 
        '#attributes' => array('onClick' => 'return false;'),
      );      
      $i++;
    }
  }
  // add one more blank field
  $form['properties']['new_id'] = array(
    '#type'          => 'select',
    '#options'       => $properties,
  );
  $form['properties']['new_value'] = array(
    '#type'          => 'textarea',
    '#default_value' => '',
    '#cols'          => 5,
    '#rows'          => $rows
  );
  $form['properties']["add"] = array(
    '#type'         => 'image_button',      
    '#value'        => t('Add'),
    '#src'          => drupal_get_path('theme', 'tripal') . '/images/add.png',
    '#ahah' => array(
      'path'    => "tripal_pub/properties/add",
      'wrapper' => 'chado-pub-details',
      'event'   => 'click',
      'method'  => 'replace',          
    ),   
    '#attributes' => array('onClick' => 'return false;'),
  );    
  /*
  $form['volume'] = array(
    '#type' => 'textfield',
    '#title' => t('Volume'),
    '#default_value' => $d_volume
  );
  
  $form['issue'] = array(
    '#type' => 'textfield',
    '#title' => t('Issue'),
    '#default_value' => $d_issue
  );

  $form['pages'] = array(
    '#type' => 'textfield',
    '#title' => t('Pages'),
    '#description' => t('Page number range[s], e.g. 457--459, viii + 664pp, lv--lvii.'),
    '#default_value' => $d_pages
  );

  $form['volumetitle'] = array(
    '#type' => 'textfield',
    '#title' => t('Volume Title'),
    '#description' => t('Title of part if one of a series.'),
    '#default_value' => $d_volumetitle
  );
  $form['miniref'] = array(
    '#type' => 'textfield',
    '#title' => t('Mini-Ref'),
    '#required' => FALSE,
    '#default_value' => $d_miniref
  );

  $form['publisher'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher Name'),
    '#required' => FALSE,
    '#default_value' => $d_publisher
  );

  $form['pubplace'] = array(
    '#type' => 'textfield',
    '#title' => t('Place of Publication'),
    '#required' => FALSE,
    '#default_value' => $d_pubplace
  );
  */
  
  $form['is_obsolete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is Obsolete? (Check for Yes)'),
    '#required' => TRUE,
    '#default_value' => $d_isobsolete
  );
  return $form;

}
/*
 * 
 */
function theme_chado_pub_node_form($form) {
  $rows = array();
  if ($form['properties']) {
    foreach ($form['properties'] as $i => $ranks) {
      if (is_numeric($i)) {    
        foreach ($ranks as $rank => $elements) {
          if (is_numeric($rank)) {          
            $rows[] = array(  
              array('data' => drupal_render($elements["prop_id-$i-$rank"]), 'width' => '20%'),    
              drupal_render($elements["prop_value-$i-$rank"]),
              array('data' => drupal_render($elements["remove-$i-$rank"]), 'width' => '5%'),
            );
          }
        }
      }
    }
    $rows[] = array(
      array('data' => drupal_render($form['properties']['new_id']), 'width' => '20%'),    
      drupal_render($form['properties']['new_value']),
      array('data' => drupal_render($form['properties']['add']), 'width' => '5%'),
    ); 
  }  
  $headers = array('Property Type','Value', '');
  
  $markup = '<div id="chado-pub-details">';
  $markup .= drupal_render($form['pub_id']);
  $markup .= drupal_render($form['title']);
  $markup .= drupal_render($form['type_id']);
  $markup .= drupal_render($form['series_name']);
  $markup .= drupal_render($form['pyear']);
  $markup .= drupal_render($form['uniquename']);
  $markup .= "<b>Include Additional Details</b>";
  $markup .= theme('table', $headers, $rows);
  $markup .= "</div>";
  
  $form['properties'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
  );
  return drupal_render($form);
}

/*
 * 
 */
function tripal_pub_property_add() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('chado_pub_node_form', $form);  

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}
/*
 * 
 */
function tripal_pub_property_delete() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('chado_pub_node_form', $form);  

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}