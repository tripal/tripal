<?php

class go__gene_expression extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Gene expression';

  // The default description for this field.
  public static $description = 'The process in which a gene\'s sequence is converted into a
      mature gene product or products (proteins or RNA). This includes the production of an
      RNA transcript as well as any processing to produce a mature RNA product or an
      mRNA (for protein-coding genes) and the translation of that mRNA into protein.
      Protein maturation is included when required to form an active form of a product from
      an inactive precursor form.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'GO',
    // The name of the term.
    'term_name' => 'gene expression',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0010467',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  ];

  // The default widget for this field.
  public static $default_widget = 'go__gene_expression_widget';

  // The default formatter for this field.
  public static $default_formatter = 'go__gene_expression_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;

  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'type' => 'xs:string',
        'readonly' => TRUE,
      ],
    ];
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {

    $record = $entity->chado_record;
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $entity->chado_table;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    $linker_table = $base_table . '_expression';
    $options = [
      'return_array' => 1,
    ];
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $exp_linkers = $record->$linker_table;
    if ($exp_linkers) {
      foreach ($exp_linkers as $i => $exp_linker) {

        // Because the unqiuename is a text field we must expand it.
        $expression = $exp_linker->expression_id;
        $expression = chado_expand_var($expression, 'field', 'expression.uniquename', $options);

        // Set the values that will be seen by the user on the page and in
        // web services, or anwhere this field is viewed.
        $entity->{$field_name}['und'][$i]['value'] = [
          'name' => $expression->uniquename,
          'description' => $expression->description,
          //'md5checksum' => $expression->md5checksum,
        ];

        // Add the pub information if a real pub is associated with the record.
        $pub = $exp_linker->pub_id;
        if ($pub->uniquename != 'null') {
          $pub_details = chado_get_minimal_pub_info($pub);

          $entity->{$field_name}['und'][$i]['value']['publication'] = $pub_details;
          $entity->{$field_name}['und'][$i]['value']['publication']['type'] = $pub->type_id->name;
          if (property_exists($pub, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['publication'][0]['value']['entity'] = 'TripalEntity:' . $pub->entity_id;
          }
        }

        // Add the linker_expressionprop
        $linkerprop_table = $linker_table . 'prop';
        if (chado_table_exists($linkerprop_table)) {
          $exp_linker = chado_expand_var($exp_linker, 'table', $linkerprop_table, $options);
          $exp_linkerprops = $exp_linker->feature_expressionprop;
          if ($exp_linkerprops) {
            foreach ($exp_linkerprops AS $linkerprop) {
              $entity->{$field_name}['und'][$i]['value'][$linkerprop->type_id->name] = $linkerprop->value;
            }
          }
        }
        // Add the fields for the widget form.  The name requres the following
        // format if the fields should be used as values for insertint/updating
        // into the chado table:  [table_name]__[field_name]
        $entity->{$field_name}['und'][$i][$linker_table . '__expression_id'] = $expression->expression_id;
        $entity->{$field_name}['und'][$i][$linker_table . '__uniquename'] = $expression->uniquename;
        //$entity->{$field_name}['und'][$i][$linker_table . '__md5checksum'] = $expression->md5checksum;
        $entity->{$field_name}['und'][$i][$linker_table . '__description'] = $expression->description;

      }
    }
  }
}