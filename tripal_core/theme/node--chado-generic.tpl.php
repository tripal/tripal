<?php
// Purpose: This template provides a generic layout for all Tripal nodes (page)

// get the template type of node (e.g. if type == chado_organism then template type == organism)
$ttype = $variables['type'];
$ttype = preg_replace('/chado_/','', $ttype);

if ($teaser) { 
  print theme('tripal_' . $ttype . '_teaser', $variables); 
} 
else { ?>


<script type="text/javascript">
(function ($) {
  Drupal.behaviors.<?php print $ttype?>Behavior = {
    attach: function (context, settings){ 
      $(".tripal-data-block").hide();
 
      // iterate through all of the info boxes and add their titles
      // to the table of contents
      $(".tripal-info-box-title").each(function(){
        var parent = $(this).parent();
        var id = $(parent).attr('id');
        var title = $(this).text();
        $('#tripal_<?php print $ttype?>_toc_list').append('<div class="tripal_toc_list_item"><a href="#'+id+'" class="tripal_<?php print $ttype?>_toc_item">'+title+'</a></div>');
      });

      // when a title in the table of contents is clicked, then
      // show the corresponding item in the details box
      $(".tripal_<?php print $ttype?>_toc_item").click(function(){
        $(".tripal-data-block").hide();
        href = $(this).attr('href');
        if(href.match(/^#/)){
           //alert("correct: " + href);
        }
        else{
          tmp = href.replace(/^.*?#/, "#");
          href = tmp;
          //alert("fixed: " + href);
        }
        $(href).parent().fadeIn('slow');

        return false;
      }); 

      // we want the base details to show up when the page is first shown 
      // unless we're using the feature browser then we want that page to show
      var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
      if(block == null){
        block = window.location.href.match(/[\?|\&]block=(.+)/)
      }
      if(block != null){
        var parent =  $("#tripal_<?php print $ttype?>-"+block[1]+"-box").parent();
        parent.show();
      }
      else {
        var parent = $("#tripal_<?php print $ttype?>-base-box").parent();
        parent.show();
      }
    }
  };
})(jQuery);
</script>

<div id="tripal_<?php print $ttype?>_content" class="tripal-contents"> <?php 
  if ($page['tripal_sidebar']) { ?>
    <div id="tripal-sidebar" class="column sidebar">
      <div class="section">
        <?php print render($page["chado_" . $ttype . " _toc"]); ?>
      </div>
    </div><?php 
  } ?>
  <table id="tripal-contents-table">
    <tr class="tripal-contents-table-tr">
      <td nowrap class="tripal-contents-table-td tripal-contents-table-td-toc"  align="left">
        <div id="tripal_<?php print $ttype?>_toc_list" class="tripal_toc_list">
        
         <!-- Resource Links CCK elements --><?php
         if(property_exists($node, 'field_resource_links')) {
           for($i = 0; $i < count($node->field_resource_links); $i++){
             if($node->field_resource_links[$i]['value']){
               $matches = preg_split("/\|/",$node->field_resource_links[$i]['value']);?>
               <li><a href="<?php print $matches[1] ?>" target="_blank"><?php print $matches[0] ?></a></li><?php
             }
           }
         }
         ?> 
          </div>
        </td>
        <td class="tripal-contents-table-td-data" align="left" width="100%">
         <!-- Resource Blocks CCK elements --> <?php
         if (property_exists($node, 'field_resource_titles')) {
           for ($i = 0; $i < count($node->field_resource_titles); $i++){
             if ($node->field_resource_titles[$i]['value']){ ?>
               <div id="tripal_<?php print $ttype?>-resource_<?php print $i?>-box" class="tripal_<?php print $ttype?>-info-box tripal-info-box">
                 <div class="tripal_<?php print $ttype?>-info-box-title tripal-info-box-title"><?php print $node->field_resource_titles[$i]['value'] ?></div>
                 <?php print $node->field_resource_blocks[$i]['value']; ?>
               </div> <?php
             }
           } 
         }?>
         <!-- Let modules add more content -->
         <?php
           foreach ($content as $key => $values) {
             if (array_key_exists('#value', $values)) {
               print $content[$key]['#value'];
             }
           }
         ?>
      </td>
    </tr>
  </table>
</div> 
<?php 
} ?>

