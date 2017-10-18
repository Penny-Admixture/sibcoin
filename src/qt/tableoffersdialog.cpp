#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"
#include "convertdexdata.h"

TableOffersDialog::TableOffersDialog(DexDB *db, const TypeOffer &type, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);

    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertDexData::toListQtOfferInfo(db->getOffersBuy());
    } else {
        offers = ConvertDexData::toListQtOfferInfo(db->getOffersSell());
    }

    pModel = new OfferModel(offers);

    pDetails = new OfferDetails(this);

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel(pModel);

    initComboPayment();
    initComboCountry();
    initComboCurrency();

    connect(ui->cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(ui->cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(ui->cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);

    connect(ui->tableView, &QTableView::doubleClicked, this, &TableOffersDialog::clickedColumn);

    changedFilterCountryIso(0);
    changedFilterCurrencyIso(0);
    changedFilterPaymentMethod(0);
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

void TableOffersDialog::initComboPayment()
{
    auto payments = db->getPaymentMethodsInfo();
    auto itPayment = payments.begin();
    while (itPayment != payments.end()) {
        ui->cBoxPayment->addItem(tr(itPayment->second.name.c_str()), itPayment->first);
        ++itPayment;
    }
}

void TableOffersDialog::initComboCountry() {
    auto countries = db->getCountriesInfo();
    auto itCountry = countries.begin();

    QMap<QString, QString> countrySort;
    while (itCountry != countries.end()) {
        countrySort[tr(itCountry->second.name.c_str())] = QString::fromUtf8(itCountry->first.c_str());
        ++itCountry;
    }

    auto itCountrySort = countrySort.begin();
    while (itCountrySort != countrySort.end()) {
        ui->cBoxCountry->addItem(itCountrySort.key(), itCountrySort.value());
        ++itCountrySort;
    }
}

void TableOffersDialog::initComboCurrency()
{
    auto currencies = db->getCurrenciesInfo();
    auto itCurrency = currencies.begin();

    QMap<QString, QString> currencySort;
    while (itCurrency != currencies.end()) {
        currencySort[tr(itCurrency->second.name.c_str())] = QString::fromUtf8(itCurrency->first.c_str());
        ++itCurrency;
    }

    auto itCurrencySort = currencySort.begin();
    while (itCurrencySort != currencySort.end()) {
        ui->cBoxCurrency->addItem(itCurrencySort.key(), itCurrencySort.value());
        ++itCurrencySort;
    }
}

void TableOffersDialog::changedFilterCountryIso(const int &) {
    QString iso = ui->cBoxCountry->currentData().toString();
    pModel->setFilterCountryIso(iso);
}

void TableOffersDialog::changedFilterCurrencyIso(const int &) {
    QString iso = ui->cBoxCurrency->currentData().toString();
    pModel->setFilterCurrencyIso(iso);
}

void TableOffersDialog::changedFilterPaymentMethod(const int &)
{
    quint8 payment = ui->cBoxPayment->currentData().toInt();
    pModel->setFilterPaymentMethod(payment);
}

void TableOffersDialog::clickedColumn(QModelIndex index)
{
    if (index.column() == 3) {
        QtOfferInfo info = pModel->offerInfo(index.row());
        QString country = ui->cBoxCountry->currentText();
        QString currency = ui->cBoxCurrency->currentText();
        QString payment = ui->cBoxPayment->currentText();
        pDetails->setOfferInfo(info, country, currency, payment);
        pDetails->show();
    }
}
