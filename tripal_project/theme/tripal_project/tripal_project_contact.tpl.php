<?php
$project = $variables['node']->project;

// expand the project object to include the contacts from the project_contact
// table in chado.
$project = tripal_core_expand_chado_vars($project,'table','project_contact', array('return_array' => 1));
$contacts = $project->project_contact;

if (count($contacts) > 0) { ?>
  <div id="tripal_project-contacts-box" class="tripal_project-info-box tripal-info-box">
    <div class="tripal_project-info-box-title tripal-info-box-title">People</div>
    <div class="tripal_project-info-box-desc tripal-info-box-desc">The following people particpated in development or execution of this project</div><?php     
    $i = 0; 
    foreach ($contacts as $contact) { ?>
      <b><?php print $contact->contact_id->name ?></b>, <?php print $contact->contact_id->description ?>
      <table id="tripal_project-contacts-table" class="tripal_project-table tripal-table tripal-table-horz"> <?php
      
        // expand the contact to include the properties.  This table doesn't
        // actually exist in Chado v1.11 or Chado v1.2. But, for some sites it has been
        // added manually, and it is expected that this table will be added to fiture
        // versions of Chado, so the code is included below to handle contact properties.
        $contact = tripal_core_expand_chado_vars($contact,'table','contactprop');       
        if ($contact->contactprop) {
          foreach ($contact->contactprop as $prop) {
             $class = 'tripal-table-odd-row';
             if ($i % 2 == 0 ) {
               $class = 'tripal-table-even-row';
             }
             # make the type a bit more reader friendly
             $type = $prop->type_id->name;
             $type = preg_replace("/_/", " ", $type);
             $type = ucwords($type);
             ?>
             <tr class="<?php print $class ?>">
               <td> <?php print $type ?></td>
               <td> <?php print $prop->value ?></td>
             </tr> <?php
             $i++;  
          } 
        }
        /* else { ?>
          <tr class="tripal-table-odd-row">
            <td>No contact information available for <?php print $contact->contact_id->name ?></td>
          </tr> <?php
        } */
        ?>
      </table> <?php 
    } ?>    
  </div> <?php 
}
