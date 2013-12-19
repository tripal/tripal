<?php
// this template does not follow the typical Tripal API. Normally
// variables are expanded using the tripal_core_expand_chado_vars API
// function call, but expanding the relationships table does not yeild
// a meaningful order to the data.  Therefore, relationships are preprocessed
// into an array named 'all_relationships', which is used in the template below.

$contact = $variables['node']->contact;

$all_relationships = $contact->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

// make the contact type a bit more human readable
$contact_type =  preg_replace("/_/", ' ', $contact->type_id->name);

if (count($object_rels) > 0 or count($subject_rels) > 0) {
?>
  <div id="tripal_contact-relationships-box" class="tripal_contact-info-box tripal-info-box">
    <div class="tripal_contact-info-box-title tripal-info-box-title">Relationships</div>
    <!--  <div class="tripal_contact-info-box-desc tripal-info-box-desc"></div> --><?php
    
      // first add in the subject relationships.  
      foreach ($subject_rels as $rel_type => $rels){
         // make the type a bit more human readable
         $rel_type = preg_replace("/_/", ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through each parent   
         foreach ($rels as $obj_type => $objects){?>
           <p>This contact is a <b><?php print $rel_type ?></b> of the following contact(s):
           <table id="tripal_contact-relationships_as_object-table" class="tripal_contact-table tripal-table tripal-table-horz">
             <tr>
               <th>contact Name</th>
             </tr> <?php
             foreach ($objects as $object){ ?>
               <tr>
                 <td><?php 
                    if ($object->nid) {
                      print "<a href=\"" . url("node/" . $object->nid) . "\" target=\"_blank\">" . $object->name . "</a>";
                    }
                    else {
                      print $object->name;
                    } ?>
                 </td>
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      }
      
      // second add in the object relationships.  
      foreach ($object_rels as $rel_type => $rels){
         // make the type more human readable
         $rel_type = preg_replace('/_/', ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through the children         
         foreach ($rels as $subject_type => $subjects){?>
           <p>The following contacts are a <b><?php print $rel_type ?></b> of this contact:
           <table id="tripal_contact-relationships_as_object-table" class="tripal_contact-table tripal-table tripal-table-horz">
             <tr>
               <th>Name</th>
             </tr> <?php
             foreach ($subjects as $subject){ ?>
               <tr>
                 <td><?php 
                    if ($subject->nid) {
                      print "<a href=\"" . url("node/" . $subject->nid) . "\" target=\"_blank\">" . $subject->name . "</a>";
                    }
                    else {
                      print $subject->name;
                    } ?>
                 </td>
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      } ?>
  </div> <?php
}
