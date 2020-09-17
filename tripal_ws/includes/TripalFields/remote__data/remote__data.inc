<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class remote__data extends WebServicesField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------
  // The default label for this field.
  public static $default_label = 'Remote Tripal Site';

  // The default description for this field.
  public static $default_description = 'Allows for inclusion of remote data from another Tripal site.';

  // The default widget for this field.
  public static $default_widget = 'remote__data_widget';

  // The default formatter for this field.
  public static $default_formatter = 'remote__data_formatter';

  // The module that manages this field.
  public static $module = 'tripal_ws';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_tripal_ws_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'Thing',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'property',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // Settings to allow the site admin to set the remote data source info.
    'data_info' => [
      'query' => '',
      'remote_site' => '',
      'description' => '',
      'rd_field_name' => '',
      'site_logo' => '',
    ],
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = TRUE;

  // Holds an object describing the remote site that tihs field connects to.
  private $remote_site = NULL;

  // Set to TRUE if this field is being loaded via web services. WE don't
  // want remote fields loaded when a web-service call is made.
  private $loaded_via_ws = FALSE;

  public function __construct($field, $instance) {
    parent::__construct($field, $instance);

    // This field should not do anything if it is loaded via web-services.
    // We don't want remote content to be available in web services.  There
    // is an if statement to not show this field in the web services but the
    // entity_load function doesn't know this field shouldn't be loaded so
    // we need to short-circuit that.
    if (preg_match('/web-services/', $_SERVER['REQUEST_URI'])) {
      $this->loaded_via_ws = TRUE;
      return;
    }

    // Get the site url from the tripal_sites table.
    if (array_key_exists('data_info', $instance['settings'])) {
      $site_id_ws = $instance['settings']['data_info']['remote_site'];
      if ($site_id_ws) {
        $this->remote_site = db_select('tripal_sites', 'ts')
          ->fields('ts')
          ->condition('ts.id', $site_id_ws)
          ->execute()
          ->fetchObject();
      }
    }
  }

  /**
   * @see WebServicesField::load()
   */
  public function load($entity) {

    // If this field is being loaded via web services then just return.
    if ($this->loaded_via_ws == TRUE) {
      return;
    }

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      'remote_entity' => NULL,
      'error' => FALSE,
      'warning' => FALSE,
      'admin_message' => '',
      'query_str' => '',
    ];

    // Get the query set by the admin for this field and replace any tokens
    $query_str = $this->instance['settings']['data_info']['query'];
    $bundle = tripal_load_bundle_entity(['name' => $entity->bundle]);
    $query_str = tripal_replace_entity_tokens($query_str, $entity, $bundle);

    // Make the request.
    $data = $this->makeRemoteRequest($query_str);
    $context = [];
    if (is_array($data['@context'])) {
      $contenxt = $data['@context'];
    }
    else {
      $context = json_decode(file_get_contents($data['@context']), TRUE);
      $context = $context['@context'];
    }

    if (!$data) {
      $entity->{$field_name}['und'][0]['value'] = 'ERROR: there was a problem retrieving content for this field.';
      $entity->{$field_name}['und'][0]['admin_message'] = "The remote service returned no data.";
      $entity->{$field_name}['und'][0]['remote_entity'] = NULL;
      $entity->{$field_name}['und'][0]['error'] = TRUE;
      $entity->{$field_name}['und'][0]['warning'] = FALSE;
      $entity->{$field_name}['und'][0]['query_str'] = $this->buildRemoteURL($this->remote_site, $query_str);
      return;
    }
    // Make sure we didn't have a problem
    if (array_key_exists('error', $data)) {
      $entity->{$field_name}['und'][0]['value'] = 'ERROR: there was a problem retrieving content for this field.';
      $entity->{$field_name}['und'][0]['admin_message'] = "The  content is currently not available because the " .
        "remote service reported the following error: " . $data['error'] . ".";
      $entity->{$field_name}['und'][0]['remote_entity'] = NULL;
      $entity->{$field_name}['und'][0]['error'] = TRUE;
      $entity->{$field_name}['und'][0]['warning'] = FALSE;
      $entity->{$field_name}['und'][0]['query_str'] = $this->buildRemoteURL($this->remote_site, $query_str);
      return;
    }

    $num_items = count($data['member']);
    if ($num_items == 0) {
      $entity->{$field_name}['und'][0]['value'] = 'Content is unavailable on the remote service.';
      $entity->{$field_name}['und'][0]['admin_message'] = "The query to the remote service returned an empty result set. If you " .
        "think this is an error, please check the query string and the remote service to verify. ";
      $entity->{$field_name}['und'][0]['warning'] = TRUE;
      $entity->{$field_name}['und'][0]['error'] = FALSE;
      $entity->{$field_name}['und'][0]['remote_entity'] = NULL;
      $entity->{$field_name}['und'][0]['query_str'] = $this->buildRemoteURL($this->remote_site, $query_str);
      return;
    }

    // Iterate through the members returned and save those for the field.
    for ($i = 0; $i < $num_items; $i++) {
      $member = $data['member'][$i];

      // Get the cotent type and remote entity id
      $content_type = $member['@type'];
      $remote_entity_id = $member['@id'];
      $remote_entity_id = preg_replace('/^.*\/(\d+)/', '$1', $remote_entity_id);

      // Separate the query_field if it has subfields.
      $rd_field_name = $this->instance['settings']['data_info']['rd_field_name'];
      $subfields = explode(',', $rd_field_name);
      $query_field = $subfields[0];

      // Next get the the details about this member.
      $query_field_url = $context[$content_type] . '/' . $remote_entity_id . '/' . $query_field;
      $field_data = $this->makeRemoteRequest($query_field_url);

      // If we encounter any type of error, we'll reset the field and return.
      if ($field_data && array_key_exists('error', $field_data)) {
        $entity->{$field_name} = [];
        $entity->{$field_name}['und'][0]['value'] = 'ERROR: there was a problem retrieving secific content for this field.';
        $entity->{$field_name}['und'][0]['admin_message'] = "While iterating through the list of results, the " .
          "remote service reported the following error: " . $field_data['error'] . ". ";
        $entity->{$field_name}['und'][0]['remote_entity'] = NULL;
        $entity->{$field_name}['und'][0]['error'] = TRUE;
        $entity->{$field_name}['und'][0]['warning'] = FALSE;
        $entity->{$field_name}['und'][0]['query_str'] = $this->buildRemoteURL($this->remote_site, $query_field_url);
        return;
      }

      // Set the field data as the value.
      $field_data_type = $field_data['@type'];
      $entity->{$field_name}['und'][$i]['value'] = $field_data;
      $entity->{$field_name}['und'][$i]['remote_entity'] = $member;
      $entity->{$field_name}['und'][$i]['error'] = FALSE;
      $entity->{$field_name}['und'][$i]['warning'] = FALSE;
      $entity->{$field_name}['und'][$i]['admin_message'] = '';
      $entity->{$field_name}['und'][$i]['query_str'] = $this->buildRemoteURL($this->remote_site, $query_field_url);;
    }
  }

  /**
   * Used to build the full URL for the query.
   */
  private function buildRemoteURL($remote_site, $query) {
    $path = $query;
    $q = '';
    if (preg_match('/\?/', $query)) {
      list($path, $q) = explode('?', $query);
    }

    if (empty($remote_site)) {
      tripal_report_error('tripal_ws', TRIPAL_ERROR, 'Unable to find remote_site in remote__data field while attempting to build the remote URL.');
      return NULL;
    }

    return tripal_build_remote_content_url($remote_site, $path, $q);
  }

  /**
   * Makes a request to a remote Tripal web services site.
   *
   * @param $query
   *   The query string. This string is added to the URL for the remote
   *   website.
   *
   * @return array on success or null if request fails.
   */
  private function makeRemoteRequest($query) {
    $path = $query;
    $q = '';
    if (preg_match('/\?/', $query)) {
      list($path, $q) = explode('?', $query);
    }

    if (empty($this->remote_site)) {
      tripal_report_error('tripal_ws', TRIPAL_ERROR, 'Unable to find remote_site while attempting to make the request.');
      return NULL;
    }

    try {
      $data = tripal_get_remote_content($this->remote_site->id, $path, $q);
    } catch (Exception $exception) {
      tripal_report_error('tripal_ws', TRIPAL_ERROR, $exception->getMessage());
      return NULL;
    }

    return $data;
  }

  /**
   *
   * @see TripalField::settingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();

    // Get the setting for the option for how this widget.
    $instance = $this->instance;
    $settings = '';
    $site_list = '';

    $tokens = [];
    // Get the form info from the bundle about to be saved.
    $bundle = tripal_load_bundle_entity(['name' => $instance['bundle']]);
    // Retrieve all available tokens.
    $tokens = tripal_get_entity_tokens($bundle);

    $element['data_info'] = [
      '#type' => 'fieldset',
      '#title' => 'Remote Data Settings',
      '#description' => 'These settings allow you to provide a Tripal web
        services query to identify content on another Tripal site and display
        that here within this field.  You must specify the query to execute and
        the field to display.',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#prefix' => "<div id='set_titles-fieldset'>",
      '#suffix' => '</div>',
    ];

    // Get the site info from the tripal_sites table.
    $sites = db_select('tripal_sites', 's')
      ->fields('s')
      ->execute()->fetchAll();

    foreach ($sites as $site) {
      $rows[$site->id] = $site->name;
    }

    $element['data_info']['remote_site'] = [
      '#type' => 'select',
      '#title' => t('Remote Tripal Site'),
      '#options' => $rows,
      '#default_value' => $this->instance['settings']['data_info']['remote_site'],
    ];

    $element['data_info']['query'] = [
      '#type' => 'textarea',
      '#title' => 'Query to Execute',
      '#description' => 'Enter the query that will retreive the remote records. ',
      '#default_value' => $this->instance['settings']['data_info']['query'],
      '#rows' => 3,
      '#required' => TRUE,
    ];
    $element['data_info']['query_instructions'] = [
      '#type' => 'fieldset',
      '#title' => 'Query to Execute Instructions',
      '#description' => 'If the full URL to the remote tripal content web ' .
        'service is "https://[tripal_site]/web-services/content/v0.1/". Then ' .
        'this field should contain the text immediately after the ' .
        '"content/v0.1" portion of the URL. For information about building ' .
        'web services queries see the online documentation at ' .
        l('The Tripal v3 User\'s Guide', 'http://tripal.info/tutorials/v3.x/web-services') .
        '. For example, suppose this field is attached to an Organism content ' .
        'type on the local site, and you want to retrieve a field for the ' .
        'same organism on a remote Tripal site. To retrieve the matching ' .
        'record, you will want to query on the genus and species, since it ' .
        'is unique and, you want them to match the organism for each specific ' .
        'local organism page. You can use tokens to do this (see the ' .
        '"Available Tokens" fieldset below). For this example, the full ' .
        'remote web service endpoint would be ' .
        '"https://[tripal_site]/web-services/content/v0.1/Organism" ' .
        'and the query text should be ' .
        '"Organism?genus=[taxrank__genus]&species=[taxrank__species]".',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];
    $element['data_info']['query_instructions']['instructions'] = [
      '#type' => 'markup',
      '#markup' => '',
    ];

    $element['data_info']['token_display']['tokens'] = [
      '#type' => 'hidden',
      '#value' => serialize($tokens),
    ];

    $element['data_info']['token_display'] = [
      '#type' => 'fieldset',
      '#title' => 'Available Tokens',
      '#description' => 'Copy the token and paste it into the "Query" text field above.',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    $element['data_info']['token_display']['content'] = [
      '#type' => 'item',
      '#markup' => theme_token_list($tokens),
    ];

    $element['data_info']['rd_field_name'] = [
      '#type' => 'textfield',
      '#title' => 'Field to Display',
      '#description' => 'Enter the key from the results returned by the "Query to Execute" that should be displayed. See the example below for more details.',
      '#default_value' => $this->instance['settings']['data_info']['rd_field_name'],
      '#required' => TRUE,
    ];

    $element['data_info']['rd_field_name_instructions'] = [
      '#type' => 'fieldset',
      '#title' => 'Field to Display Instructions',
      '#description' => '',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];
    $element['data_info']['rd_field_name_instructions']['insructions'] = [
      '#type' => 'markup',
      '#markup' => 'The query from a Tripal web service response is always
       in JSON array arranged in key/value pairs.  The key is the name of a
       controlled vocabulary term.  
       <br><br>Suppose you want to query details about an organism.
       Consider the following JSON result 
       <pre style="height: 200px; overflow: auto;">   
        "@type": "organism",
        "label": "Anopheles gambiae",
        "ItemPage": "http://demo.tripal.info/3.x/bio_data/642",
        "type": "Organism",
        "abbreviation": "A.gambiae",
        "genus": "Anopheles",
        "species": "gambiae",
        "common_name": "mosquito",
        "database_cross_reference": "http://demo.tripal.info/3.x/web-services/content/v0.1/Organism/642/database+cross+reference",
        "equivalent_name": "Anopheles gambiae sensu stricto",
        "division": "Invertebrates",
        "mitochondrial_genetic_code_name": "Invertebrate Mitochondrial",
        "synonym": "Anopheles gambiae S",
        "genetic_code": "1",
        "lineage": "cellular organisms; Eukaryota; Opisthokonta; Metazoa; Eumetazoa; Bilateria; Protostomia; Ecdysozoa; Panarthropoda; Arthropoda; Mandibulata; Pancrustacea; Hexapoda; Insecta; Dicondylia; Pterygota; Neoptera; Holometabola; Diptera; Nematocera; Culicomorpha; Culicoidea; Culicidae; Anophelinae; Anopheles; Cellia; Pyretophorus; gambiae species complex",
        "genetic_code_name": "Standard",
        "genbank_common_name": "African malaria mosquito"
       </pre> 
       To display the "common_name" from the JSON above you would enter the word
       "common_name" in the Field to Display textbox.
      ',
    ];

    $element['data_info']['description'] = [
      '#type' => 'textarea',
      '#title' => 'Description',
      '#description' => 'Describe the data being pulled in.',
      '#default_value' => $this->instance['settings']['data_info']['description'],
      '#rows' => 1,
    ];

    $fid = $this->instance['settings']['data_info']['site_logo'] ? $this->instance['settings']['data_info']['site_logo'] : NULL;
    $file = NULL;
    if ($fid) {
      $file = file_load($fid);
    }
    $element['data_info']['site_logo'] = [
      '#title' => 'Remote Site Logo',
      '#description' => t('When data is taken from a remote site and shown to the user,
         the site from which the data was retrieved is indicated.  If you would like to
         include the logo for the remote site, please upload an image here.'),
      '#type' => 'managed_file',
      '#default_value' => $file ? $file->fid : NULL,
      '#theme' => 'image_widget',
      '#attached' => [
        'css' => [
          'image-preview' => drupal_get_path('module', 'image') . '/image.css',
        ],
      ],
      'preview' => [
        '#markup' => theme('image_style', [
          'style_name' => 'thumbnail',
          'path' => $file ? $file->uri : '',
        ]),
      ],
    ];

    return $element;
  }

  /**
   * @see TripalField::instanceSettingsFormValidate()
   */
  public function instanceSettingsFormValidate($form, &$form_state) {
    $site_logo = $form_state['values']['instance']['settings']['data_info']['site_logo'];

    // If we have a site logo then add usage information.
    if ($site_logo) {
      $file = file_load($site_logo);
      $file_usage = file_usage_list($file);
      if (!array_key_exists('tripal_ws', $file_usage)) {
        file_usage_add($file, 'tripal_ws', 'site-logo', 1);
      }
    }
  }


}
