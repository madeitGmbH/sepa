defmodule SepaTest do
  use ExUnit.Case
  doctest Sepa
  doctest Sepa.Nif

  describe "to_xml" do
    test "cti" do
      cti = %Sepa.CreditTransferInitiation{
        name: "Max Mustermann",
        iban: "DE56510108001234567890",
        bic: "BANKDEFFXXX",
        transactions: [
          %{
            name: "Erika Mustermann",
            amount: Decimal.new("12.55"),
            iban: "DE56510108001234567890",
            bic: "BANKDEZZXXX"
          }
        ]
      }

      assert {:ok, xml} = Sepa.to_xml(cti)

      assert xml =~ "urn:iso:std:iso:20022:tech:xsd:pain.001"

      assert xml =~ "Max Mustermann"
      assert xml =~ "DE56510108001234567890"
      assert xml =~ "BANKDEFFXXX"

      assert xml =~ "Erika Mustermann"
      assert xml =~ "12.55"
      assert xml =~ "DE56510108001234567890"
      assert xml =~ "BANKDEZZXXX"
    end
  end
end
