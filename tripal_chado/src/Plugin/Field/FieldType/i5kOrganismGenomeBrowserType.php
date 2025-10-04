<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;


/**
 * Plugin implementation of i5k Organism Genome Browser field type.
 */
#[FieldType(
  id: 'i5k_organism_genome_browser_type',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Organism Genome Browser'),
  description: new TranslatableMarkup('A link to this organism\'s Genome Browser'),
  default_widget: 'i5k_organism_genome_browser_widget',
  default_formatter: 'i5k_organism_genome_browser_formatter',
)]
class i5kOrganismGenomeBrowserType extends ChadoFieldItemBase {

  public static $id = "i5k_organism_genome_browser_type";

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];
    return $elements + parent::fieldSettingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['max_length'] = 255;
    $settings['storage_plugin_settings']['base_table'] = '';
    $settings['storage_plugin_settings']['base_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];
    return $elements + parent::storageSettingsForm($form,$form_state,$has_data);
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    return $constraints;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the settings for this field.
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the length of the database fields so we don't go over the size limit.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $organism_def = $schema->getTableDef('organism', ['format' => 'Drupal']);
    $cvterm_def = $schema->getTableDef('cvterm', ['format' => 'Drupal']);
    $genus_len = $organism_def['fields']['genus']['size'];
    $species_len = $organism_def['fields']['species']['size'];
    $scientific_name_len = $genus_len + $species_len + 3;

    // Get the base table columns needed for this field.
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];
    $base_fk_col = array_keys($base_schema_def['foreign keys']['cvterm']['columns'])[0];

    // Get the CV terms used for each of the properties
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
#    $drupal_entity_term = 'schema:ItemPage';
    $organism_id_term = $mapping->getColumnTermId($base_table, 'organism_id');
    $genus_term = $mapping->getColumnTermId('organism', 'genus');
    $species_term = $mapping->getColumnTermId('organism', 'species');
#    $scientific_name_term = 'NCBITaxon:scientific_name';
    $url_term = 'schema:url';

    // Assemble the properties for this field.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'path' => $base_table . '.' . $base_pkey_col,
    ]);
#    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'entity_id', $drupal_entity_term, [
#      'action' => 'function',
#      'drupal_store' => TRUE,
#      'namespace' => self::$chadostorage_namespace,  // the namespace of the parent class Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage
#      'function' => self::$drupal_entity_callback,  // i.e. drupalEntityIdLookupCallback
#      'fkey' => 'organism_id',
#    ]);
#    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'organism_id', $organism_id_term, [
#      'action' => 'store',
#      'drupal_store' => TRUE,
#      'path' => $base_table . '.' . $base_fk_col,
#    ]);
#    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_genus', $genus_term, $genus_len, [
#      'action' => 'store',
#      'drupal_store' => FALSE,
#      'path' => $base_table . '.organism_id>organism.organism_id;genus',
#    ]);
#    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'organism_species', $species_term, $species_len, [
#      'action' => 'store',
#      'drupal_store' => FALSE,
#      'path' => $base_table . '.organism_id>organism.organism_id;species',
#    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'jbrowse_title', $url_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => static::class,
      'function' => 'generateTitle',
#      'fkey' => 'organism_id',
    ]);
    $properties[] = new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'jbrowse_link', $url_term, [
      'action' => 'function',
      'drupal_store' => TRUE,
      'namespace' => static::class,
      'function' => 'generateUrl',
#      'fkey' => 'organism_id',
    ]);
$thing = -1;
#dpm($properties, "CP30 properties");//@@@
    return $properties;
  }

  /**
   * A callback function to generate a Title for a JBrowse URL.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return string
   *   A title to be displayed to the site user.
   */
  static public function generateTitle($context): string {

    $delta = $context['delta'];
    $values = $context['values'];
    $genus = $values['organism_genus'][$delta]['value']['value']->getValue();
    $species = $values['organism_species'][$delta]['value']['value']->getValue();
    $title = 'JBrowse Viewer for ' . $genus . ' ' . $species;
print "CP33 title ".$title."\n";
    return $title;
  }

  /**
   * A callback function to generate a JBrowse URL.
   *
   * @param array $context
   *   Values that a callback function might need in order
   *   to calculate the field's final value.
   *
   * @return string
   *   A url, or an empty string if one is not avaliable for this organism.
   */
  static public function generateUrl($context): string {

    $delta = $context['delta'];
    $values = $context['values'];
    $genus = $values['organism_genus'][$delta]['value']['value']->getValue();
    $species = $values['organism_species'][$delta]['value']['value']->getValue();
    $url = 'https://apollo.nal.usda.gov/apollo/' . $genus . '_' . $species . '/jbrowse';
print "CP34 url ".$url."\n";
    return $url;
  }

}
