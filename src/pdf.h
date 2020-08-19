#pragma once

namespace vpl{

int savepdf(vpl::PageInfo& info){

    using namespace std;

    PDFWriter pdfWriter;
    EStatusCode status;

    do
    {
        status = pdfWriter.StartPDF( info.filename+"_.pdf", ePDFVersion14);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to start PDF\n";
            break;
        }

        PDFPage* page = new PDFPage();
        // set document to the size of the image (setting proportional makes things super small)
        page->SetMediaBox(PDFRectangle(0, 0, info.width, info.height));

        PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
        if (NULL == pageContentContext)
        {
            status = PDFHummus::eFailure;
            cout << "failed to create content context for page\n";
        }

        // place the image on top...hopefully we can see soem transparency
        AbstractContentContext::ImageOptions imageOptions;
        // too small
        // imageOptions.transformationMethod = AbstractContentContext::EImageTransformation::eFit;
        // imageOptions.fitProportional = true;

        // imageOptions.transformationMethod = AbstractContentContext::eMatrix;
        // imageOptions.matrix[0] = imageOptions.matrix[3] = 0.5;   // scale image by half
        pageContentContext->DrawImage(0, 0, info.filename, imageOptions);  // problems reading the file

        status = pdfWriter.EndPageContentContext(pageContentContext);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to end page content context\n";
            break;
        }

        status = pdfWriter.WritePageAndRelease(page);
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed to write page\n";
            break;
        }


        status = pdfWriter.EndPDF();
        if (status != PDFHummus::eSuccess)
        {
            cout << "failed in end PDF\n";
            break;
        }
    } while (false);
    return status;
}
}// end namepace vpl
