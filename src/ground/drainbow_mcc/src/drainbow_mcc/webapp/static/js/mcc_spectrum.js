function updateSpectrum(spectrumUpdateData) {
    $.getJSON(spectrumUpdateData.dataUrl, function(data) {
        console.log("updating spectrum");


    });

    document.getElementById("spectrum-img").src = spectrumUpdateData.IMGUrl + '?timestamp_ms=' + Date.now()

    spectrumUpdateData.timeoutContext.setTimeout(
        function() {
             updateSpectrum(spectrumUpdateData);
        },
        updatePeriodSpectrumMs
    );
}

function mccSpectrumMain(spectrumDataUrl, spectrumIMGUrl)
{
    spectrumUpdateData = {
        dataUrl: spectrumDataUrl,
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