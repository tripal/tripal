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
  public static $default_settings = array(
    'storage' => 'field_tripal_ws_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => array(),
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
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
      'data_info' => array(
        'query' => '',
        'remote_site' => '',
        'description' => '',
      ),
  );
  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;
  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

  // Holds an object describing the remote site that tihs field connects to.
  private $remote_site = NULL;

  public function __construct($field, $instance) {
    parent::__construct($field, $instance);

    //Get the site url from the tripal_sites table.
    $site_id_ws = $this->instance['settings']['data_info']['remote_site'];
    $this->remote_site = db_select('tripal_sites', 'ts')
      ->fields('ts')
      ->condition('ts.id', $site_id_ws)
      ->execute()
      ->fetchObject();
  }
  /**
   * @see WebServicesField::load()
   */
  public function load($entity) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    // Get the query set by the admin for this field and replace any tokesn
    $query_str = $this->instance['settings']['data_info']['query'];
    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $query_str = tripal_replace_entity_tokens($query_str, $entity, $bundle);

    // Make the request.
    $data = $this->makeRemoteRequest($query_str);
    if(!$data){
     return;
    }

    $total_items = $data['totalItems'];

    // Iterate through the members returned and save those for the field.
    for ($i = 0; $i < count($data['members']); $i++) {
      $member = $data['members'][$i];

      // Get the cotent type and remote entity id
      $content_type = $member['@type'];
      $remote_entity_id = $member['@id'];
      $remote_entity_id = preg_replace('/^.*\/(\d+)/', '$1', $remote_entity_id);

      // Save the member information for use later.
      $entity->{$field_name}['und'][$i]['remote_entity'] = $member;

      // Next get the the details about this member.
      $query_field = 'relationship';
      $query_field_url =  $content_type . '/' . $remote_entity_id . '/' . $query_field;
      $field_data = $this->makeRemoteRequest($query_field_url);
      if(!$field_data){
        // If we encounter any type of error, we'll reset the field and return.
        $entity->{$field_name}['und'] = array();
        $entity->{$field_name}['und'][0] = array(
          'value' => array(),
        );
        return;
      }

      // Set the field data as the value.
      $field_data_type = $field_data['@type'];
      $entity->{$field_name}['und'][$i]['value'] = $field_data;
    }
  }
  /**
   * Makes a request to a remote Tripal web services site.
   *
   * @param $query
   *   The query string. This string is added to the URL for the remote
   *   website.
   */
  private function makeRemoteRequest($query) {

    // Build the URL to the remote web services.
    $ws_url = $this->remote_site->url;
    $ws_url = trim($ws_url, '/');
    $ws_url .= '/web-services/content/v0.1';

    // Build the Query and make and substitions needed.
    $query_url = $ws_url . '/' . $query;
    $options = array('timeout' => 360);
    $data = drupal_http_request($query_url, $options);

    if (!$data) {
      tripal_report_error('tripal_ws', TRIPAL_ERROR,
          t('Could not connect to the remote web service.'));
      return FALSE;
    }

    // If the data object has an error then this is some sort of
    // connection error (not a Tripal web servcies error).
    if (property_exists($data, 'error')) {
      tripal_report_error('tripal_ws', TRIPAL_ERROR,
          'Web Services error on remote site: %error.',
          array('%error' => $data->error));
      return FALSE;
    }

    // We got a response, so convert it to a PHP array.
    $data = drupal_json_decode($data->data);

    // Check if there was a Tripal Web Services error.
    if (array_key_exists('error', $data)) {
      $error = '</pre>' . print_r($data['error'], TRUE) . '</pre>';
      tripal_report_error('tripal_ws', TRIPAL_ERROR,
        'Web Services error on remote site: %error.',
        array('%error' => $error));
      return FALSE;
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

    $tokens = array();
    // Get the form info from the bundle about to be saved.
    $bundle_info = tripal_load_bundle_entity(array('name' => $instance['bundle']));

    // Retrieve all available tokens.
    $tokens = tripal_get_entity_tokens($bundle_info);

    $element['data_info'] = array(
      '#type' => 'fieldset',
      '#title' => 'Remote Data Settings',
      '#description' => 'Provide the site name, query and description for the remote data source.',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#prefix' => "<div id='set_titles-fieldset'>",
      '#suffix' => '</div>',
    );

    // Get the site info from the tripal_sites table.
    $sites = db_select('tripal_sites', 's')
      ->fields('s')
      ->execute()->fetchAll();

    foreach ($sites as $site) {
      $rows[$site->id] =$site->name;
    }

    $element['data_info']['remote_site'] = array(
      '#type' => 'select',
      '#title' => t('Site'),
      '#options' => $rows,
      '#default_value' => $this->instance['settings']['data_info']['remote_site'],
    );

    $element['data_info']['query'] = array(
      '#type' => 'textarea',
      '#title' => 'Query',
      '#description' => 'Build the query string that should be appended after the url. The tokens
      listed below may be used in your query build.',
      '#default_value' => $this->instance['settings']['data_info']['query'],
      '#rows' => 5
    );

    $element['data_info']['token_display']['tokens'] = array(
      '#type' => 'hidden',
      '#value' => serialize($tokens)
    );

    $element['data_info']['token_display'] = array(
      '#type' => 'fieldset',
      '#title' => 'Available Tokens',
      '#description' => 'Copy the token and paste it into the "Query" text field above.',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE
    );

    $element['data_info']['token_display']['content'] = array(
      '#type' => 'item',
      '#markup' => theme_token_list($tokens),
    );

    $element['data_info']['description'] = array(
      '#type' => 'textarea',
      '#title' => 'Description',
      '#description' => 'Describe the data being pulled in.',
      '#default_value' =>  $this->instance['settings']['data_info']['description'],
      '#rows' => 1
    );
    /*$element['test_button']['data'] = array(
      '#prefix' => '<div class="returned-data">',
      '#suffix' => '</div>',
      '#type' => 'container',
        //…
    );

    $element['test_button'] = array(
      '#type' => 'button',
      '#value' => t('Test Query'),
      '#ajax' => array(
        'wrapper' => 'data-id',
        'callback' => 'tripal_ws_url_query_test',
      ),
        //…
    );*/

      return $element;
  }

    /**
     * Ajax callback.
     */
    function tripal_ws_url_query_test_ajax($form, $form_state) {
      load();
      return $element['test_button']['data'];
    }
 /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function instanceSettingsFormValidate($form, &$form_state) {
  }


}
