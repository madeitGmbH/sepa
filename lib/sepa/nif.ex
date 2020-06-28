defmodule Sepa.Nif do
  @moduledoc false
  @on_load :load_nifs

  def load_nifs do
    :erlang.load_nif('./priv/sepa', 0)
  end

  @doc """
  ## Examples

      iex> match?({:ok, _}, Sepa.Nif.create_message(1))
      true

      iex> match?({:ok, _}, Sepa.Nif.create_message(2))
      true

  """
  def create_message(_type) do
    raise "NIF create_message/1 not implemented"
  end

  @doc """
  ## Examples

      iex> {:ok, msg} = Sepa.Nif.create_message(2)
      iex> Sepa.Nif.set_name(msg, 'Max Mustermann')
      :ok

  """
  def set_name(_msg, _name) do
    raise "NIF set_name/2 not implemented"
  end

  @doc """
  ## Examples

      iex> {:ok, msg} = Sepa.Nif.create_message(2)
      iex> Sepa.Nif.set_iban(msg, 'DE22380200904030000000')
      :ok

  """
  def set_iban(_msg, _iban) do
    raise "NIF set_iban/2 not implemented"
  end

  @doc """
  ## Examples

      iex> {:ok, msg} = Sepa.Nif.create_message(2)
      iex> Sepa.Nif.set_bic(msg, 'BYLADEM1ERH')
      :ok

  """
  def set_bic(_msg, _bic) do
    raise "NIF set_bic/2 not implemented"
  end

  @doc """
  """
  def add(_msg, _tx) do
    raise "NIF add/2 not implemented"
  end

  @doc """
  ## Examples

      iex> {:ok, msg} = Sepa.Nif.create_message(2)
      iex> match?({:ok, _}, Sepa.Nif.to_xml(msg))
      true

  """
  def to_xml(_msg) do
    raise "NIF to_xml/1 not implemented"
  end

  @doc """
  ## Examples

      iex> Sepa.Nif.check_iban("DE22380200904030000000")
      true
      iex> Sepa.Nif.check_iban("DE22380200904030000001")
      false

  """
  def check_iban(_iban) do
    raise "NIF check_iban/1 not implemented"
  end

  @doc """
  ## Examples

      iex> Sepa.Nif.bic_get_bank_name("BYLADEM1ERH")
      {:ok, "STADT-UND KREISSPARKASSE ERLANGEN HOECHSTADT HERZOGENAURACH"}
      iex> Sepa.Nif.bic_get_bank_name("BYLADAM1ERH")
      :error

  """
  def bic_get_bank_name(_iban) do
    raise "NIF bic_get_bank_name/1 not implemented"
  end

  @doc """
  ## Examples

      iex> Sepa.Nif.iban_convert(<<"DE", 0>>, <<"7325022", 0>>, <<"26580070", 0>>)
      {:ok, "DE32265800700732502200", 1}
      iex> Sepa.Nif.iban_convert(<<"DE", 0>>, <<"40033086", 0>>, <<"30020500", 0>>)
      {:ok, "DE02500202000040033086", 15}
      iex> Sepa.Nif.iban_convert(<<"DE", 0>>, <<"123", 0>>, <<"123", 0>>)
      :error

  """
  def iban_convert(_country, _accounts, _bank_id) do
    raise "NIF iban_convert/3 not implemented"
  end
end
