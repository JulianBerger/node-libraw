var fs = require('fs');
var expect = require('chai').expect;
var libraw = require('./libraw');

describe('LibRAW', function() {
  describe('Extracting thumbnail', function () {
    var outputFilename = '';

    it('should extract the thumbnail', function () {
      libraw.extractThumb('./test.raf', './output')
        .then(function(output) {
          expect(output).to.equal('./output.thumb.jpg');
          outputFilename = output;
        });
    });

    it('should check that the file has been properly created', function(done) {
      fs.access(outputFilename, fs.F_OK, function(err) {
        expect(err).to.be.null;
        done();
      });
    });

    it('should delete the file after creation', function(done) {
      fs.unlink(outputFilename, done);
    });
  });

  describe('Extracting TIFF', function () {
    var outputFilename = '';

    it('should extract the tiff', function () {
      this.timeout(90000);

      libraw.extract('./test.raf', './output')
        .then(function(output) {
          expect(output).to.equal('./output.tiff');
          outputFilename = output;
        });
    });

    it('should check that the file has been properly created', function(done) {
      fs.access(outputFilename, fs.F_OK, function(err) {
        expect(err).to.be.null;
        done();
      });
    });

    it('should delete the file after creation', function(done) {
      fs.unlink(outputFilename, done);
    });
  });

  describe('Extraction to a Buffer', function() { 
    this.timeout(5000);

    it('should return a thumbnail buffer', function(done) {
      libraw.extractThumbBuffer('./test.raf').then(function(buffer) {
        expect(buffer.length).to.be.greaterThan(2048 * 1024);
        done();
      })
    });

    it('should return a full size buffer', function(done) {
      this.timeout(90000);
      libraw.extractBuffer('./test.raf').then(function(buffer) {
        expect(buffer.length).to.be.greaterThan(1000000);
        done();
      })
    });
  })

  it('should read exif data from a file', function() {
    libraw.getExif('./test.raf')
      .then(function(exif) {
        expect(exif['Orientation']).to.be.equal("Rotate 90 CW")
      });
  });
});
