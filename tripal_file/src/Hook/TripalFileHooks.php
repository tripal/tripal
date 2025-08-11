<?php

namespace Drupal\tripal_file\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;

/**
 * Hook implementations for the TripalFile module.
 */

class TripalFileHooks {

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match) {
    switch ($route_name) {
      // Main module help for the tripal_file module.
      case 'help.page.tripal_file':
        $output = '<h3>' . t('About') . '</h3>';
        $output .= '<p>' . t('A module for associating files with Tripal content and for accessing files via web services.') . '</p>';
        return $output;
      default:
    }
  }

  /**
   * Implements hook_chado_core_mapping_alter().
   *
   * Adds terma to linker tables defined by this module.
   */
  #[Hook('chado_core_mapping_alter')]
  function chado_core_mapping_alter(&$config) {
    print "CP81 hook chado_core_mapping_alter called\n";

    $new_linker_tables = [
      'file_contact',
      'file_license',
      'file_pub',
      'analysis_file',
      'assay_file',
      'biomaterial_file',
      'cv_file',
      'eimage_file',
      'feature_file',
      'featuremap_file',
      'library_file',
      'nd_protocol_file',
      'organism_file',
      'phylotree_file',
      'project_file',
      'pub_file',
      'stock_file',
      'stockcollection_file',
      'study_file',
    ];
#tripal_chado.chado_tripal_file_mapping.tripal_file_mapping
      /** @var Drupal\Core\Config\Entity\ConfigEntityStorage **/
      $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
      /** @var Drupal\tripal_chado\Entity\ChadoTermMapping **/
      self::$mapping = $storage->load('core_mapping');

# notice] Added field, "file_type", to content type: "tripal_file".
# warning] Undefined array key "primary key" TripalLicenseTypeDefault.php:85
# warning] Undefined array key "fields" TripalLicenseTypeDefault.php:89
# warning] Trying to access array offset on null TripalLicenseTypeDefault.php:89
# warning] Trying to access array offset on null TripalLicenseTypeDefault.php:89
# error]  TypeError: Drupal\tripal_chado\TripalField\ChadoFieldItemBase::getColumnTermId(): Argument #2 ($column) must be of type string, null given, called in /var/www/drupal/web/modules/contrib/tripal/tripal_file/src/Plugin/Field/FieldType/TripalLicenseTypeDefault.php on line 118 in Drupal\tripal_chado\TripalField\ChadoFieldItemBase::getColumnTermId() (line 1204 of /var/www/drupal/web/modules/contrib/tripal/tripal_chado/src/TripalField/ChadoFieldItemBase.php) #0 /var/www/drupal/web/modules/contrib/tripal/tripal_file/src/Plugin/Field/FieldType/TripalLicenseTypeDefault.php(118): Drupal\tripal_chado\TripalField\ChadoFieldItemBase::getColumnTermId('file', NULL, 'SIO:000729')
print "CP82:";var_dump(array_keys($config));
# just integers print "CP83:";var_dump(array_keys($config['tables']));
# print "CP84:";var_dump(array_keys($config['tables']['project_contact'] ?? []));
print "CP85:";var_dump($config['tables'][50] ?? []);
#CP82:array(4) {
#  [0]=>
#  string(2) "id"
#  [1]=>
#  string(5) "label"
#  [2]=>
#  string(11) "description"
#  [3]=>
#  string(6) "tables"
#}
#CP85:array(2) {
#  ["name"]=>
#  string(6) "dbprop"
#  ["columns"]=>
#  array(4) {
#    [0]=>
#    array(3) {
#      ["name"]=>
#      string(5) "db_id"
#      ["term_id"]=>
#      string(11) "ERO:0001716"
#      ["term_name"]=>
#      string(8) "database"
#    }
#    [1]=>
#    array(3) {
#      ["name"]=>
#      string(4) "rank"
#      ["term_id"]=>
#      string(12) "OBCS:0000117"
#      ["term_name"]=>
#      string(10) "rank order"
#    }
#    [2]=>
#    array(3) {
#      ["name"]=>
#      string(7) "type_id"
#      ["term_id"]=>
#      string(21) "schema:additionalType"
#      ["term_name"]=>
#      string(14) "additionalType"
#    }
#    [3]=>
#    array(3) {
#      ["name"]=>
#      string(5) "value"
#      ["term_id"]=>
#      string(11) "NCIT:C25712"
#      ["term_name"]=>
#      string(5) "Value"
#    }
#  }
#}
  }

}
