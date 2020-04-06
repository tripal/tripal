
Forms (user input)
====================

In Drupal 8, all the parts of the form are contained within a single class. The ``buildForm`` method is where you define what your form will look like. This is done using the Form API which is very similar to what was used in Drupal 7. The ``validateForm`` method allows you to validate the user submitted data and give feedback to the user and the ``submitForm`` method allows you to act on the data submitted.

  Defining forms as structured arrays, instead of as straight HTML, has many advantages including:

    - Consistent HTML output for all forms.
    - Forms provided by one module can be easily altered by another without complex search and replace logic.
    - Complex form elements like file uploads and voting widgets can be encapsulated in reusable bundles that include both display and processing logic.

  *Excerpt from* `Official Drupal Docs: Introduction to Form API <https://www.drupal.org/docs/8/api/form-api/introduction-to-form-api>`_

Tripal uses the Drupal Form API without any modification and your form classes will be saved, one per file, in the ``src/forms`` directory of your extension module.

Additional Resources:
 - `Official Drupal Docs: Introduction to Form API <https://www.drupal.org/docs/8/api/form-api/introduction-to-form-api>`_
 - `Official Drupal Docs: Form API <https://www.drupal.org/docs/8/api/form-api>`_
 - `Karim Boudjema: Create a custom form with Form API in Drupal 8 <http://karimboudjema.com/en/drupal/20181013/create-custom-form-form-api-drupal-8>`_
 - `Jaywant Topno: Step by step method to create a custom form in Drupal 8 <https://www.valuebound.com/resources/blog/step-by-step-method-to-create-a-custom-form-in-drupal-8>`_
 - `Official Drupal Docs: Upgrading forms from Drupal 7 <https://www.drupal.org/node/1932058>`_
