<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Render\FormattableMarkup;

/**
 * Plugin implementation of an organism scientific name formatter.
 */
#[TripalFieldFormatter(
  id: 'i5k_organism_genome_browser_formatter',
  label: new TranslatableMarkup('Organism Genome Browser Formatter'),
  description: new TranslatableMarkup('A link to this organism\'s Genome Browser'),
  field_types: [
    'i5k_organism_genome_browser_type',
  ],
)]
class i5kOrganismGenomeBrowserFormatter extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach ($items as $delta => $item) {
      $values = [
        'title' => $item->get('jbrowse_title')->getString(),
        'uri' => $item->get('jbrowse_link')->getString(),
      ];

      $url = Url::fromUri($values['uri']);
      $link = Link::fromTextAndUrl($values['title'], $url)->toString();

      $elements[] = [
        '#markup' => $link,
      ];
    }

    return $elements;
  }

}
