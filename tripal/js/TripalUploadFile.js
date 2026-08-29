/**
 * TripalUploadFile Class, handles upload of one file.
 */
(function (jQuery, Drupal) {

  "use strict";

  class TripalUploadFile {

    /**
     * Class constructor function.
     */
    constructor(file, options) {

      this.file = file;
      this.options = options;
      this.file_size = file.size;
      this.chunk_size = (1024 * 2000);
      this.total_chunks = ((this.file.size % this.chunk_size === 0) ? Math.floor(this.file.size / this.chunk_size) : Math.floor(this.file.size / this.chunk_size) + 1);
      this.curr_chunk = 0;
      this.status = 'pending';
      this.file_id = null;

      if ('mozSlice' in file) {
        this.slice_method = 'mozSlice';
      }
      else if ('webkitSlice' in file) {
        this.slice_method = 'webkitSlice';
      }
      else {
        this.slice_method = 'slice';
      }

      this.xhr = new XMLHttpRequest();
      this.xhr.onload = () => {
        this._onChunkComplete();
      }

      // Respond to changes in connection.
      if ('onLine' in navigator) {
        window.addEventListener('online', this._onConnectionFound.bind(this));
        window.addEventListener('offline', this._onConnectionLost.bind(this));
      }
    }

    /**
     * Upload one chunk of the file.
     */
    _upload() {
      // Calculate the range for the current chunk.
      let range_start = this.curr_chunk * this.chunk_size;
      let range_end = range_start + this.chunk_size;

      // If we've gone beyond the number of chunks then just quit.
      if (this.curr_chunk > this.total_chunks) {
        this._onChunkComplete();
        return;
      }

      // Prevent range overflow.
      if (range_end > this.file_size) {
        range_end = this.file_size;
      }

      const chunk = this.file[this.slice_method](range_start, range_end);
      const url = this.options.url + '/' + this.file.name + '/save/' + this.curr_chunk;

      this.xhr.open('POST', url, true);
      this.xhr.overrideMimeType('application/octet-stream');
      this.xhr.setRequestHeader('Content-Range', 'bytes ' + range_start + '-' + range_end + '/' + this.file_size);

      this.xhr.send(chunk);
    }

    /**
     * Converts a file size into a human readable value.
     *
     * Borrowed function from:
     * http://stackoverflow.com/questions/10420352/converting-file-size-in-bytes-to-human-readable
     */
    _getReadableSize(bytes, si) {
      const thresh = si ? 1000 : 1024;

      if (Math.abs(bytes) < thresh) {
        return bytes + ' B';
      }
      const units = si
        ? ['kB','MB','GB','TB','PB','EB','ZB','YB']
        : ['KiB','MiB','GiB','TiB','PiB','EiB','ZiB','YiB'];
      let u = -1;
      do {
        bytes /= thresh;
        ++u;
      }
      while (Math.abs(bytes) >= thresh && u < units.length - 1);
      return bytes.toFixed(1) + ' ' + units[u];
    }

    /**
     * Queries server to see what chunk the loading left off at.
     */
    async _checkUpload() {
      try {
        const url = `${this.options.url}/${this.file.name}/check/`;

        const params = new URLSearchParams({
          module: this.options.module,
          chunk_size: this.chunk_size,
          file_size: this.file_size,
        });

        const response = await fetch(
          `${url}?${params.toString()}`,
          {
            headers: {
              Accept: 'application/json',
            },
          }
        );

        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }

        const data = await response.json();

        if (data.status === 'failed') {
          this.status = 'failed';
          this.updateStatus();
          alert(data.message);
          return;
        }

        this.curr_chunk = data.curr_chunk;
        this.status = 'uploading';
        this._upload();
        this.updateStatus();
        this.updateProgressBar();
      }
      catch (error) {
        alert(error.message);
        this.curr_chunk = 0;
        this._upload();
      }
    }

    /**
     * Merged uploaded chunks to the final file.
     */
    _mergeChunks() {

      const url = `${this.options.url}/${this.file.name}/merge`;

      const params = new URLSearchParams({
        module: this.options.module,
        file_size: this.file_size,
      });

      fetch(`${url}?${params.toString()}`, {
        method: 'GET',
        headers: {
          'Accept': 'application/json',
        },
      })
      .then((response) => {
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        return response.json();
      })
      .then((data) => {

        if (data.status === 'completed') {
          this.file_id = data.file_id;
          this.status = 'completed';
          this.updateStatus();
        }
        else {
          this.status = 'failed';
          this.updateStatus();
          alert(data.message);
        }
      })
      .catch(() => {
        this.status = 'failed';
        this.updateStatus();
      });
    }

    /**
     * Event handler for completion of one chunk.
     */
    _onChunkComplete() {
      // If the curr_chunk and the total_chunks is the same then
      // we've reached the end.
      if (this.curr_chunk >= this.total_chunks) {
        this.updateStatus();
        this._onUploadComplete();

        return;
      }

      // Continue as long as we aren't paused
      if (this.status === 'uploading') {
        this._upload();
        this.curr_chunk++;
        this.updateProgressBar();
      }
    }

    /**
     * Event handler for completion of upload.
     */
    _onUploadComplete() {
      this.status = 'merging';
      this._mergeChunks();
      this.updateStatus();
    }

    /**
     * When a connection has been lost but re-established then resume uploads.
     */
    _onConnectionFound() {
      this.resume();
    }

    /**
     * When a connection has been lost then pause uploads.
     */
    _onConnectionLost() {
      this.pause();
    }

    /**
     * Return a progress bar.
     */
    getProgressBar() {
      const progress_id = this.options['progress'];
      return '<div id="' + progress_id + '" class="tripal-uploader-progress-label">0%</div>';
    }

    /**
     * Return links.
     */
    getLinks() {
      const links_id = this.options['links'];
      return '<div id="' + links_id + '" class="tripal-uploader-links">0%</div>';
    }

    /**
     * Return the file category.
     */
    getCategory() {
      return this.options['category'];
    }

    /**
     * Return the file index.
     */
    getIndex() {
      return this.options['index'];
    }

    /**
     * Return the HTML table name.
     */
    getTName() {
      return this.options['tname'];
    }

    /**
     * Return the file name.
     */
    getFileName() {
      return this.file.name;
    }

    /**
     * Return the size of the file, optionally in human-readable form.
     */
    getFileSize(readable) {
      if (readable) {
        return this._getReadableSize(this.file.size, true);
      }
      else {
        return this.file.size;
      }
    }

    /**
     * Updates the links, status text and status bar.
     */
    updateStatus() {

      const progress_id = this.options['progress'];

      // Add the progress text.
      jQuery('#' + progress_id).html('');
      if (this.status === 'cancelled') {
        jQuery("<span>", {
          'text' : 'Cancelled',
        }).appendTo('#' + progress_id)
      }
      else if (this.status === 'checking') {
        jQuery("<span>", {
          'text' : 'Checking...',
        }).appendTo('#' + progress_id)
      }
      else if (this.status === 'merging') {
        jQuery("<span>", {
          'text' : 'Processing...',
        }).appendTo('#' + progress_id)
      }
      else if (this.status === 'failed') {
        jQuery("<span>", {
          'text' : 'Failed',
        }).appendTo('#' + progress_id)
      }
      else if (this.status === 'completed') {
        jQuery("<span>", {
          'text' : 'Complete',
        }).appendTo('#' + progress_id)
        // Set the parent's target field.
        const parent = this.options.parent;
        const tname = this.options.tname;
        parent.setTarget(tname);
      }
      else if (this.status === 'paused') {
        jQuery("<span>", {
          'text' : 'Paused',
        }).appendTo('#' + progress_id)
      }

      // Add a throbber if the status is uploading
      if (this.status === 'uploading' || this.status === 'checking' || this.status === 'merging') {
        jQuery("<img>", {
           'src': tripal_path + '/images/ajax-loader.gif',
           'class' : 'tripal-uploader-chunked-file-progress-throbber',
         }).appendTo('#' + progress_id);
      }

      // Add the appropriate links.
      const links_id = this.options.links;
      const category = this.options.category;
      jQuery('#' + links_id).html('');
      if (this.status === 'cancelled') {
        jQuery("<a>", {
          'id': links_id + '-pending',
          'class': category + '-pending',
          'href': 'javascript:void(0);',
          'text': 'Restore',
        }).appendTo('#' + links_id);
        jQuery('#' + links_id + '-pending').on('click', () => {
          this.pending();
        })
      }
      if (this.status === 'pending') {
        jQuery("<a>", {
          'id': links_id + '-cancel',
          'class': category + '-cancel',
          'href': 'javascript:void(0);',
          'text': 'Cancel',
        }).appendTo('#' + links_id);
        jQuery('#' + links_id + '-cancel').on('click', () => {
          this.cancel();
        })
      }
      if (this.status === 'uploading') {
        jQuery("<a>", {
          'id': links_id + '-pause',
          'class': category + '-pause',
          'href': 'javascript:void(0);',
          'text': 'Pause',
        }).appendTo('#' + links_id);
        jQuery('#' + links_id + '-pause').on('click', () => {
          this.pause();
        })
      }
      if (this.status === 'paused') {
        jQuery("<a>", {
          'id': links_id + '-resume',
          'class': category + '-resume',
          'href': 'javascript:void(0);',
          'text': 'Resume',
        }).appendTo('#' + links_id);
        jQuery('#' + links_id + '-resume').on('click', () => {
          this.resume();
        })
      }

      // Add the remove link.
      jQuery("<a>", {
        'id': links_id + '-remove',
        'class': category + '-remove',
        'href': 'javascript:void(0);',
        'text': ' Remove',
      }).appendTo('#' + links_id);
      jQuery('#' + links_id + '-remove').on('click', () => {
        const parent = this.options.parent;
        parent.removeFile(
          this.options.tname,
          this.options.category,
          this.options.index
        );
        parent.updateTable(this.options.category);
        // Unset the parent's target field.
        parent.setTarget(this.options.tname);
        this.cancel();
      })
    }

    /**
     * Updates the status bar progress only.
     */
    updateProgressBar() {
      const progress_id = this.options['progress'];
      const progress = (this.curr_chunk / this.total_chunks) * 100;

      // Calculate the amount of the file transferred.
      let size_transferred = this.curr_chunk * this.chunk_size;
      size_transferred = this._getReadableSize(size_transferred, true);

      if (this.status === 'uploading') {
        jQuery('#' + progress_id).html('');
        jQuery("<span>", {
          'class': 'tripal-uploader-chunked-file-progress-label',
          'text': size_transferred,
        }).appendTo(jQuery("<div>", {
          'id': progress_id + '-bar',
          'class': 'tripal-uploader-chunked-file-progress',
          'width': progress + '%'
        }).appendTo(jQuery("<div>", {
          'id': progress_id + '-box',
          'class': 'tripal-uploader-chunked-file-progress',
        }).appendTo('#' + progress_id)));

      }
      if (this.status === 'uploading' || this.status === 'checking' || this.status === 'merging') {
        jQuery("<img>", {
           'src': tripal_path + '/images/ajax-loader.gif',
           'class' : 'tripal-uploader-chunked-file-progress-throbber',
         }).appendTo('#' + progress_id);
      }

    }

    /**
     * Cancel an upload.
     */
    cancel() {
      this.status = 'cancelled';
      this.updateStatus();
    }

    /**
     *
     */
    pending() {
      this.status = 'pending';
      this.updateStatus();
    }

    /**
     * Start an upload.
     */
    start() {
      if (this.status === 'pending') {
        // Change the status to checking. The first thing we'll
        // do is see what's already present on the server.
        this.status = 'checking';
        this._checkUpload();
      }
    }

    /**
     * Pause an upload.
     */
    pause() {
      this.status = 'paused';
      this.updateStatus();
    }

    /**
     * Resume a paused upload.
     */
    resume() {
      this.status = 'uploading';
      this.updateStatus();
      this.updateProgressBar();
      this._upload();
    }
  }

  // Export the objects to Drupal for use in other JS files.
  Drupal.TripalUploadFile = TripalUploadFile;

})(jQuery, Drupal);
