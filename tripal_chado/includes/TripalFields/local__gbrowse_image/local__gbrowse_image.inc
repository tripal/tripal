<?php

class local__gbrowse_image extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'GBrowse View';

  // The default description for this field.
  public static $description = 'A GBrowse image of a feature alignment to another sequence.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'Gbrowse Image',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'gbrowse_image',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,    
    // The URL for the GBrowse image
    'gbrowse_url' => '',
  );

  // The default widget for this field.
  public static $default_widget = 'local__gbrowse_image_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__gbrowse_image_formatter';


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => TRUE,
      ),
    );
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $feature = $entity->chado_record;
    
    $record = $entity->chado_record;
    $settings = $this->instance['settings'];

    // Set some defauls for the empty record
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
    );
    
    // If we don't have a record then just return
    if (!$record) {
      return;
    }
    
    // Replace any tokens in the string.
    $gbrowse_url = NULL;
    $g_urls = $settings['gbrowse_url'];   
    $g_urls = explode("\n", $g_urls);
    foreach ($g_urls as $g_url) {
      $matches = [];
      if (preg_match('/^(.*)\s+(.*)\s+(http.*)$/', $g_url, $matches)) {
        if ($record->organism_id->genus == $matches[1] and
            $record->organism_id->species == $matches[2]) {
          $gbrowse_url = $matches[3];
        }
      }
      elseif (preg_match('/^http.*$/', $g_url)) {
        $gbrowse_url = $g_url;
      }
    }
    if (!$gbrowse_url) {
      return;
    }
    $bundle = tripal_load_bundle_entity(['name' => $this->instance['bundle']]);
    $gbrowse_url = tripal_replace_entity_tokens($gbrowse_url, $entity, $bundle);
        
    // Use the term associated to the db.url table of Chado.
    $url_term = chado_get_semweb_term('db', 'url');
    
    // Add the URL to the values
    $entity->{$field_name}['und'][0]['value'] = [
      $url_term => $gbrowse_url
    ];
  }
  
  /**
   * @see TripalField::globalSettingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();
    
    $settings = $this->instance['settings'];
    
    $element['gbrowse_url'] = array(
      '#type' => 'textarea',
      '#title' => 'GBrowse URL',
      '#description' => t('Please enter the URL for generating feature ' . 
        'images on the remote GBrowse site. You may use tokens to substitute ' .
        'field values from this content type. Open the "Available Tokens" fieldset below. ' .
        'For example, to show a view of a gene from the rice genome GBrowse the ' .
        'following URL with tokens can be used: ' . 
        'http://rice.plantbiology.msu.edu/cgi-bin/gbrowse_img/rice/?name=[schema__name];type=Landmarks%3Aregion+Genes+Rice_Annotation' . 
        '. If you need to specify different GBrowse URLs for different ' .
        'species, put the genus, species and URL all on one line with each separated ' . 
        'by a space. For example: ' . 
        '"Oryza sativa http://rice.plantbiology.msu.edu/cgi-bin/gbrowse_img/rice/?name=[schema__name];type=Landmarks%3Aregion+Genes+Rice_Annotation"'),
      '#default_value' => $settings['gbrowse_url'],
    );
    
    $element['tokens'] = array(
      '#type' => 'fieldset',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#title' => 'Available Tokens'
    );
    $headers = array('Token', 'Description');
    $rows = array();
    
    // Get the bundle tokens
    $bundle = tripal_load_bundle_entity(['name' => $this->instance['bundle']]);
    $tokens = tripal_get_entity_tokens($bundle);
    foreach ($tokens as $token) {
      $rows[] = array(
        $token['token'],
        $token['description'],
      );
    }
    
    $table_vars = array(
      'header'     => $headers,
      'rows'       => $rows,
      'attributes' => array(),
      'sticky'     => FALSE,
      'caption'    => '',
      'colgroups'  => array(),
      'empty'      => 'There are no tokens',
    );
    $element['tokens']['list'] = array(
      '#type' => 'item',
      '#markup' => theme_table($table_vars),
    );
    
    return $element;
  }
}
