function updateSpectrum(spectrumUpdateData) {
    /*$.getJSON(spectru,UpdateData.dataUrl, function(data) {
        console.log("updating spectrum");


    });*/

    document.getElementById("spectrum-img").src = spectrumUpdateData.IMGUrl + '?timestamp_ms=' + Date.now()

    spectrumUpdateData.timeoutContext.setTimeout(
        function() {
             updateSpectrum(spectrumUpdateData);
        },
        updatePeriodSpectrumMs
    );
}

function mccSpectrumMain(spectrumIMGUrl)
{
    spectrumUpdateData = {
        IMGUrl: spectrumIMGUrl,
        timeoutContext: this
    };

    setTimeout(
        function() {
             updateSpectrum(spectrumUpdateData);
        },
        updatePeriodSpectrumMs
    );
}