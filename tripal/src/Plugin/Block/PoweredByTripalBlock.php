<?php

namespace Drupal\tripal\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Provides a 'Powered by Tripal' Block.
 *
 * @Block(
 *   id = "powered_by_tripal",
 *   admin_label = @Translation("Powered by Tripal"),
 *   category = @Translation("Tripal"),
 * )
 */
class PoweredByTripalBlock extends BlockBase implements BlockPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function build() {
    $config = $this->getConfiguration();
    $size = $config['logo_size'] ?? 'small';
    $type = $config['logo_type'] ?? 'bw';
    $image = "powered_by_tripal_{$type}_{$size}.png";
    $image_path = base_path() . \Drupal::service('extension.list.module')->getPath('tripal') . '/images/' . $image;

    return [
      '#markup' => '<a href="http://tripal.info"><img border="0" src="' . $image_path . '"></a>',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form = parent::blockForm($form, $form_state);

    $config = $this->getConfiguration();

    $form['logo_size'] = [
      '#type' => 'radios',
      '#title' => $this->t('Logo Size'),
      '#description' => $this->t('Select if you would like a small or large "Powered by Tripal" logo.'),
      '#default_value' => $config['logo_size'] ?? 'small',
      '#options' => [
        'large' => $this->t('Large'),
        'small' => $this->t('Small'),
      ],
    ];

    $form['logo_type'] = [
      '#type' => 'radios',
      '#title' => $this->t('Logo Type'),
      '#description' => $this->t('Select if you would like a black and white or colorful "Powered by Tripal" logo.'),
      '#default_value' => $config['logo_type'] ?? 'bw',
      '#options' => [
        'bw' => $this->t('Gray scale'),
        'col' => $this->t('Colorful'),
      ],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function blockSubmit($form, FormStateInterface $form_state) {
    parent::blockSubmit($form, $form_state);
    $values = $form_state->getValues();
    $this->configuration['logo_size'] = $values['logo_size'];
    $this->configuration['logo_type'] = $values['logo_type'];
  }

}
