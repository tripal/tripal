<?php
/**
 * @file
 * Template file for the views integration fields form.
 */
?>

<style type="text/css">

#tripal-views-integration-form  .fields-new-row, .field-headers {
   dislay: block;
   margin: 0px;
   border-bottom-style: solid;
   border-bottom-width: 1px;
}

#tripal-views-integration-form .form-item {
   margin: 0px 0px 5px 0px;
}

#tripal-views-integration-form .column-one, .column-two, .column-three, .column-four {
   display: inline-block;
   margin: 0px;
   vertical-align: top;
   margin-left: 15px;
   /**border: 1px solid #000;*/
}

#tripal-views-integration-form  .field-headers {
   font-weight: bold;
   height: 15px;
}

#tripal-views-integration-form  .field-headers div {
   display: inline-block;
   margin: 0px;
   vertical-align: top;
   padding-left: 15px;
}

#tripal-views-integration-form .column-name {
   font-weight: bold;
}

#tripal-views-integration-form .column-type {
   font-style: italic;
}

#tripal-views-integration-form .column-one {
   width: 30%;
   height: 50px;
}

#tripal-views-integration-form .column-two, .column-four {
   width: 30%;
}

#tripal-views-integration-form .column-three {
   width: 0%;
}

#tripal-views-integration-form  .fields-new-row {
   padding-bottom: 10px;
   margin-bottom: 5px;
   padding-top: 10px;
}

#tripal-views-integration-form  .joins-new-row {
   padding-bottom: 10px;
   margin-bottom: 5px;
   padding-top: 10px;
   border-bottom-style: solid;
   border-bottom-width: 1px;
}

#tripal-views-integration-form .join-column-one, .join-column-two, .join-column-three {
   display: inline-block;
   margin: 0px;
   vertical-align: top;
   margin-left: 15px;
   width: 30%;
}

</style>

<?php print drupal_render_children($form); ?>

