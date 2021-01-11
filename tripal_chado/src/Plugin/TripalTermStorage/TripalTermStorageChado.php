<?php

namespace Drupal\tripal_chado\Plugin\TripalTermStorage;

use Drupal\tripal\Entity\TripalVocab;
use Drupal\tripal\Entity\TripalVocabSpace;
use Drupal\tripal\Entity\TripalTerm;

use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageBase;
use Drupal\tripal\Plugin\TripalTermStorage\TripalTermStorageInterface;
use Drupal\Core\Entity\EntityStorageInterface;

/**
 * TripalTerm Storage plugin: Chado Integration.
 *
 * @ingroup tripal_chado
 *
 * @TripalTermStorage(
 *   id = "chado",
 *   label = @Translation("GMOD Chado Integration"),
 *   description = @Translation("Ensures Tripal Vocabularies are linked with chado cvterms."),
 * )
 */
class TripalTermStorageChado extends TripalTermStorageBase implements TripalTermStorageInterface {

  /**
   * {@inheritdoc}
   */
  public function postSaveVocab(TripalVocab &$entity, EntityStorageInterface $storage, $update) {

    // Figure out the chado schema name.
    // @todo find a better way to do this obviously.
    $chado_schema_name = 'chado';

    // Get the TripalVocab ID.
    $tripalvocab_id = $entity->get('id')->value;

    // Get the chado cv_id.
    $chadocv['name'] = $entity->getNamespace();
    $chadocv['definition'] = $entity->getName();

    $exists = chado_select_record('cv', ['cv_id'], $chadocv, [], $chado_schema_name);
    if ($exists) {
      $cv_id = $exists[0]->cv_id;
    }
    else {
      // Because we already checked uniqueness with the select above,
      // we don't need to do validation for the insert. This is added
      // to help performance.
      $options['skip_validation'] = TRUE;
      $cv = chado_insert_record('cv', $chadocv, $options, $chado_schema_name);
      if ($cv) {
        $cv_id = $cv['cv_id'];
      }
      else {
        return FALSE;
      }
    }

    // Finally, we need to link the records.
    $connection = \Drupal::service('database');
    $result = $connection->merge('chado_tripalvocab')
      ->key('tripalvocab_id', $tripalvocab_id)
      ->fields([
        'schema_name' => $chado_schema_name,
        'cv_id' => $cv_id,
      ])
      ->execute();

  }

  /**
   * {@inheritdoc}
   */
  public function postSaveVocabSpace(TripalVocabSpace &$entity, EntityStorageInterface $storage, $update) {

    // Figure out the chado schema name.
    // @todo find a better way to do this obviously.
    $chado_schema_name = 'chado';

    // Get the TripalVocabSpace ID.
    $tripalvocabspace_id = $entity->get('id')->value;

    // Grab the default Tripal Vocabulary for the url/description.
    $tripalvocab = $entity->getVocab();

    // Get the chado db_id.
    $chadodb['name'] = $entity->getIDSpace();
    $chadodb['description'] = $tripalvocab->getDescription();
    $chadodb['url'] = $tripalvocab->getURL();
    $chadodb['urlprefix'] = $entity->getURLPrefix();

    $chadodbselect = [ 'name' => $chadodb['name'] ];
    $exists = chado_select_record('db', ['db_id'], $chadodbselect, [], $chado_schema_name);
    if ($exists) {
      $db_id = $exists[0]->db_id;
    }
    else {
      // Because we already checked uniqueness with the select above,
      // we don't need to do validation for the insert. This is added
      // to help performance.
      $options['skip_validation'] = TRUE;
      $db = chado_insert_record('db', $chadodb, $options, $chado_schema_name);
      if ($db) {
        $db_id = $db['db_id'];
      }
      else {
        return FALSE;
      }
    }

    // Finally, we need to link the records.
    $connection = \Drupal::service('database');
    $result = $connection->merge('chado_tripalvocabspace')
      ->key('tripalvocabspace_id', $tripalvocabspace_id)
      ->fields([
        'schema_name' => $chado_schema_name,
        'db_id' => $db_id,
      ])
      ->execute();

  }

  /**
   * {@inheritdoc}
   */
  public function postSaveTerm(TripalTerm &$entity, EntityStorageInterface $storage, $update) {

    // Figure out the chado schema name.
    // @todo find a better way to do this obviously.
    $chado_schema_name = 'chado';

    // Get the TripalTerm ID.
    $tripalterm_id = $entity->get('id')->value;

    // Get the Tripal Vocab and IDSpace.
    $idspace = $entity->getIDSpace();
    // We use the IDSpace to get the default vocabulary since chado can
    // only support a single cv for a given cvterm.
    $vocab = $idspace->getVocab();

    // Get the chado cvterm_id.
    $cvterm_accession = $entity->getAccession();
    $chadocvterm['db_name'] = $idspace->getIDSpace();
    $chadocvterm['id'] = $chadocvterm['db_name'] . ':' . $cvterm_accession;
    $chadocvterm['name'] = $entity->getName();
    $chadocvterm['definition'] = $entity->getDefinition();
    $chadocvterm['cv_name'] = $vocab->getNamespace();

    $dbxrefcheck = [ ':db_name' => $chadocvterm['db_name'], ':accession' => $cvterm_accession];
    $exists = chado_query('SELECT cvterm_id FROM {cvterm} cvt
      LEFT JOIN {dbxref} dbx ON dbx.dbxref_id=cvt.dbxref_id
      LEFT JOIN {db} db ON db.db_id=dbx.db_id
      WHERE db.name = :db_name AND dbx.accession = :accession',
      $dbxrefcheck, [] , $chado_schema_name)->fetchField();
    if ($exists) {
      $cvterm_id = $exists;
    }
    else {
      $cvterm = chado_insert_cvterm($chadocvterm, [], $chado_schema_name);
      if ($cvterm) {
        $cvterm_id = $cvterm->cvterm_id;
      }
      else {
        return FALSE;
      }
    }

    // Finally, we need to link the records.
    $connection = \Drupal::service('database');
    $result = $connection->merge('chado_tripalterm')
      ->key('tripalterm_id', $tripalterm_id)
      ->fields([
        'schema_name' => $chado_schema_name,
        'cvterm_id' => $cvterm_id,
      ])
      ->execute();

  }

  /**
   * {@inheritdoc}
   */
  public function loadVocab($id, TripalVocab &$entity) {
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadVocabSpace($id, TripalVocabSpace &$entity) {
    return $entity;
  }

  /**
   * {@inheritdoc}
   */
  public function loadTerm($id, TripalTerm &$entity) {
    return $entity;
  }
}
