
Theme (display)
=================

Theming is the process of customizing the display of pages or fields. Drupal 8 theming is a two part process, as described below. Tripal uses the Drupal theming system without alteration.

1. Use ``hook_theme`` to tell Drupal about your custom template. This hook implementation should go in your ``my_module.module`` file.
2. Use the preprocess hook to use prepare your variables and do any processing needed.
3. Finally, you use Twig templates to format the HTML and insert the variables prepared in the preprocess hook. No processing should be done in these templates.

Additional Resources:
 - `Official Drupal Docs: Theming in your custom module <https://www.drupal.org/docs/8/creating-custom-modules/theming>`_
 - `Official Drupal Docs: Create custom twig templates for custom module <https://www.drupal.org/docs/8/theming/twig/create-custom-twig-templates-for-custom-module>`_
 - `Official Drupal Docs: Working With Twig Templates <https://www.drupal.org/docs/8/theming/twig/working-with-twig-templates>`_
 - `Official Drupal Docs: Defining a custom theme <https://www.drupal.org/docs/8/theming>`_
